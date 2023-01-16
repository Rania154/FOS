#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//
uint32 subt(uint32 fst,uint32 nd)
{
	return(fst-nd);
}
uint32 dvd(uint32 num,uint32 num2)
{
	return(num/num2);
}
void initialize_dyn_block_system()
{
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("initialize_dyn_block_system() is not implemented yet...!!");

	//[1] Initialize two lists (AllocMemBlocksList & FreeMemBlocksList) [Hint: use LIST_INIT()]
	LIST_INIT(&AllocMemBlocksList);
	LIST_INIT(&FreeMemBlocksList);
#if STATIC_MEMBLOCK_ALLOC
	//DO NOTHING
#else
	/*[2] Dynamically allocate the array of MemBlockNodes
	 * 	remember to:
	 * 		1. set MAX_MEM_BLOCK_CNT with the chosen size of the array
	 * 		2. allocation should be aligned on PAGE boundary
	 * 	HINT: can use alloc_chunk(...) function
	 */
	uint32 mem = KERNEL_HEAP_MAX - KERNEL_HEAP_START;
	uint32 block_size = 1;
	while(block_size != PAGE_SIZE)
		block_size = block_size*2;

	MAX_MEM_BLOCK_CNT = dvd(mem ,block_size);

	MemBlockNodes=(struct MemBlock *)KERNEL_HEAP_START;

	MemBlockNodes[MAX_MEM_BLOCK_CNT];

	uint32 structSize = sizeof(struct MemBlock);

	uint32 size = ROUNDUP((MAX_MEM_BLOCK_CNT *structSize),block_size);
	uint32 perm = PERM_USED|PERM_WRITEABLE;
	uint32 sva = KERNEL_HEAP_START;

	allocate_chunk(ptr_page_directory ,sva, size , perm);

	initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);
	struct MemBlock *lstelement = LIST_LAST(&AvailableMemBlocksList);

	LIST_REMOVE(&AvailableMemBlocksList,lstelement);
	LIST_INSERT_TAIL(&FreeMemBlocksList,lstelement);

	lstelement->sva= subt(sva,-size);
	lstelement->size =subt(mem,size);
#endif
	//[3] Initialize AvailableMemBlocksList by filling it with the MemBlockNodes
	//[4] Insert a new MemBlock with the remaining heap size into the FreeMemBlocksList
}

void* kmalloc(unsigned int size)
{
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	struct MemBlock *ret = NULL;
	int end = -1;
	size = ROUNDUP(size,PAGE_SIZE);
	if ( isKHeapPlacementStrategyFIRSTFIT()){
		//firstfit
		ret =  alloc_block_FF(size);

	}
	else if (isKHeapPlacementStrategyBESTFIT()){
		//bestfit
		ret = alloc_block_BF(size);
	}
	else if(isKHeapPlacementStrategyNEXTFIT()){
		//nextfit
		ret = alloc_block_NF(size);
	}
	if (ret == NULL)
		return ret;
	int all =allocate_chunk(ptr_page_directory,ret->sva,ret->size,PERM_WRITEABLE);
	if ( all == end)
		return NULL;
	insert_sorted_allocList(ret);
	return ((void *)ret->sva);

	//change this "return" according to your answer
}

void kfree(void* virtual_address)
{
	//panic("kfree() is not implemented yet...!!");

	struct MemBlock *element = find_block( (&AllocMemBlocksList) , (uint32)virtual_address);

	if ( element == NULL){

	}
	else
	{
		LIST_REMOVE((&AllocMemBlocksList),(element));

			uint32 start= ROUNDDOWN(element->sva,PAGE_SIZE);

			uint32 end = ROUNDUP(subt(element->sva,-element->size),PAGE_SIZE);

			for(uint32 i = start;i < end; i = subt(i,-PAGE_SIZE))
			{
				unmap_frame(ptr_page_directory,i);
			}

			insert_sorted_with_merge_freeList(element);
	}
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//O(1)
	return (to_frame_info(physical_address)->references == 0 ? 0 : to_frame_info(physical_address)->va);

	//ANOTHER IMPLEMENTATION
	struct FrameInfo *frame = to_frame_info(physical_address);
	if (frame->references == 0)
		return 0;
	return frame->va;

	//9 SECS IMPLEMENTATION
	struct MemBlock *element =NULL;
	LIST_FOREACH(element, (&AllocMemBlocksList))
	{
		uint32 start= ROUNDDOWN(element->sva,PAGE_SIZE);

		uint32 end = ROUNDUP(subt(element->sva,-element->size),PAGE_SIZE);

		for(uint32 i = start;i < end; i= subt(i,-PAGE_SIZE))
		{
				uint32 *ptr = NULL;
				struct FrameInfo *temp = get_frame_info(ptr_page_directory,i, &ptr);
				if ( temp == frame)
				{
					return i;
				}
		}

	}
	return 0;
	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	uint32 *ptr_page_table = NULL;
	return (get_frame_info(ptr_page_directory,(uint32) virtual_address, (&ptr_page_table))== NULL ? 0 :virtual_to_physical(ptr_page_directory,(uint32) virtual_address));

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	// Write your code here, remove the panic and write your code
	if(new_size == 0)
	{
		kfree(virtual_address);
		return virtual_address;
	}
	else if (virtual_address == NULL)
		return kmalloc(new_size);

	struct MemBlock *element = find_block( (&AllocMemBlocksList) , (uint32)virtual_address);

	if ( element == NULL)
		return element;

	struct MemBlock *nxt = element->prev_next_info.le_next;
	struct MemBlock *prev = element->prev_next_info.le_prev;
	//don't change sva

	uint32 dif = subt(new_size,element->size);
	uint32 st  = subt(element->sva,-element->size);

	if ( element->size >= new_size)
		return (virtual_address);

	struct MemBlock *free = find_block(&FreeMemBlocksList,st);
	if ( free != NULL && dif <= free->size)
	{
		allocate_chunk(ptr_page_directory,st,dif,PERM_WRITEABLE);

//			if( new_size > element->size)
//					element->size = new_size;
//			return ((void *)element->sva);

		free->sva= subt(free->sva,-ROUNDUP(dif,PAGE_SIZE));
		free->size=subt(free->size,ROUNDUP(dif,PAGE_SIZE));

		element->size = subt(ROUNDUP(new_size,PAGE_SIZE),0);

		return ((void *)element->sva);
	}
	//change sva
	void * ptr = kmalloc(new_size);
	uint32 va = (uint32) ptr;

	if (ptr == NULL)
		return ptr;

	int copy = copy_paste_chunk(ptr_page_directory,(uint32)virtual_address,va,element->size);

	if(copy == -1)
		return NULL;

	kfree(virtual_address);

	return ((void *) va);

	LIST_REMOVE((&AllocMemBlocksList),(element));
	insert_sorted_with_merge_freeList(element);
	//panic("krealloc() is not implemented yet...!!");

}
