/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"


/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
uint32 mul(uint32 num,uint32 fact)
{
	return(num,fact);
}
uint32 add(uint32 addr,uint32 num)
{
	return(addr+num);
}
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	uint32 i;
	for(i =0;i < num_of_pages;i = add(i,1))
	{

		uint32 *pointerdst = NULL;
		uint32 pg = add(dest_va,mul(i,PAGE_SIZE));
		get_page_table(page_directory, (pg),&pointerdst);
		if(pointerdst != NULL)
		{

			if (pointerdst[PTX(pg)] != 0)
				return -1;
		}
	}
	for(i =0;i < num_of_pages;i = add(i,1))
	{
		uint32 *pointersc = NULL;
		uint32 *pointerdst = NULL;

		uint32 pg = add(dest_va,mul(i,PAGE_SIZE));
		uint32 cp = add(source_va,mul(i,PAGE_SIZE));

		get_page_table(page_directory, (cp),&pointersc);

		int perm = pt_get_page_permissions(page_directory,cp);

		struct FrameInfo *ptr = get_frame_info(page_directory,cp,&pointersc);


		map_frame(page_directory,ptr,pg,perm);
		unmap_frame(page_directory,cp);

		//pt_clear_page_table_entry(page_directory,cp);
	}

	return 0;
	//panic("cut_paste_pages() is not implemented yet...!!");
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code

	uint32 esc = ROUNDUP(add(source_va,size),PAGE_SIZE);
	uint32 edst = ROUNDUP(add(dest_va,size),PAGE_SIZE);

	uint32  eedst = add(dest_va,size);
	uint32 startdest = dest_va;

	uint32 startsc = source_va;
	uint32 endsc = add(source_va,size);

	source_va = ROUNDDOWN(source_va,PAGE_SIZE);
	dest_va = ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 i ;
	for(i =dest_va;i < edst;i= add(i,PAGE_SIZE))
	{
		uint32 *pointerdst = NULL;
		uint32 *pointersc = NULL;

		get_page_table(page_directory, (i),&pointerdst);
		struct FrameInfo *ptr = get_frame_info(page_directory,i,&pointerdst);
		if(ptr != NULL )
		{
			if ((pointerdst[PTX(i)]&PERM_WRITEABLE) != PERM_WRITEABLE)
				return -1;
		}
	}
	for(i =dest_va;i < edst;i+= PAGE_SIZE)
	{
		uint32 *pointerdst = NULL;
		uint32 *pointersc = NULL;

		get_page_table(page_directory, (i),&pointerdst);
		get_page_table(page_directory, source_va,&pointersc);

		struct FrameInfo *ptr = get_frame_info(page_directory,i,&pointerdst);
		if(ptr == NULL){

			if(pointerdst == NULL)
				create_page_table(page_directory,i);

			get_page_table(page_directory, (i),&pointerdst);
			allocate_frame(&ptr);

			uint32 perm = PERM_WRITEABLE;

			if((pointersc[PTX(source_va)]&PERM_USER) != 0)
				perm |= PERM_USER;

			map_frame(page_directory,ptr,i,perm);
		}
		ptr->references = get_frame_info(page_directory,i,&pointerdst)->references;

		source_va = add(source_va,mul(1,PAGE_SIZE));
	}
	//copy content
	i =startsc;
	while(i < endsc)
	{
		*((unsigned char *) (startdest)) = *((unsigned char *) (i));
		i = add(i,1),startdest = add(startdest,1);
	}
	return 0;
	//panic("copy_paste_chunk() is not implemented yet...!!");
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the panic and write your code
	uint32 esc = ROUNDUP(add(source_va,size),PAGE_SIZE);
	uint32 edst = ROUNDUP(add(dest_va,size),PAGE_SIZE);

	source_va = ROUNDDOWN(source_va,PAGE_SIZE);
	dest_va = ROUNDDOWN(dest_va,PAGE_SIZE);

	uint32 i ,j;
	for(i =dest_va;i < edst; i  = add(i,PAGE_SIZE))
	{
		uint32 *pointerdst = NULL;
		uint32 *pointersc = NULL;

		get_page_table(page_directory, (i),&pointerdst);

		if(pointerdst != NULL)
		{
			if (pointerdst[PTX(i)] != 0)
				return -1;
		}
	}
	i =dest_va;
	while(i < edst)
	{
		uint32 *pointerdst = NULL;
		uint32 *pointersc = NULL;

		get_page_table(page_directory, (source_va),&pointersc);

		struct FrameInfo *ptr = get_frame_info(page_directory,source_va,&pointersc);
		map_frame(page_directory,ptr,i,perms);

		source_va  = add(source_va,PAGE_SIZE);
		i  = add(i,PAGE_SIZE);
	}

	return 0;
	//panic("share_chunk() is not implemented yet...!!");
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	uint32 i;
	uint32 e= add(va,size);
	va =ROUNDDOWN(va,PAGE_SIZE);
	e = ROUNDUP(e,PAGE_SIZE);
	i = va;
	while(i < e)
	{
		uint32 *pointerdst = NULL;
		get_page_table(page_directory, (i),&pointerdst);
		if(pointerdst != NULL )
		{
			if( pointerdst[PTX(i)] != 0)
				return -1;
		}
		i = add(i,PAGE_SIZE);
	}
	i = va;
	while(i < e)
	{
			uint32 *pointerdst = NULL;
			struct FrameInfo *ptr = NULL;
			allocate_frame(&ptr);

			//ptr->va|=perms;
			map_frame(page_directory,ptr,i,perms);

			ptr->va |= i;
			i = add(i,PAGE_SIZE);
	}
	//panic("allocate_chunk() is not implemented yet...!!");
	return 0;

}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	sva = ROUNDDOWN(sva,PAGE_SIZE);
	eva = ROUNDUP(eva,PAGE_SIZE);

	uint32 i;
	int p=0,t=0;

	uint32 *lst = NULL;
	i = sva;
	for(;i<eva;)
	{
		uint32 *ptr = NULL;
		get_page_table(page_directory,(i),&ptr);
		if(ptr != NULL)
		{
			if(lst != ptr)
				t = add(t,1);

			struct FrameInfo *ptr2 = get_frame_info(page_directory,i,&ptr);

			if(ptr2 != NULL)
			{
				p = add(p,1);
			}
			lst = ptr;
		}
		i = add(i,PAGE_SIZE);

		*num_pages = p;
		*num_tables = t;

	}
	//cprintf("%d %d\n",p,t);
	//panic("calculate_allocated_space() is not implemented yet...!!");
}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calctable (uint32 addr)
{
	return(addr/(4194304) +2);
}
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	uint32 eva = add(sva,size);
	eva = ROUNDUP(eva,PAGE_SIZE);
	sva = ROUNDDOWN(sva,PAGE_SIZE);

	uint32 ret =0;
	uint32 lst = 0;

	uint32 i = sva;
	for(;i < eva;)
	{
		uint32 *ptr = NULL;
		get_page_table(page_directory,(i),&ptr);

		if(ptr != NULL)
		{
			struct FrameInfo *ptr2 = get_frame_info(page_directory,i,&ptr);
			if(ptr2 != NULL)
			{

			}
			else
			{
				ret = add(ret,1);
			}
		}
		else
		{
			ret = add(ret,1);

		}
		i = add(i,PAGE_SIZE);
	}
	i = sva;
	for(;i < eva;)
		{
			uint32 *ptr = NULL;
			get_page_table(page_directory,(i),&ptr);

			if(ptr == NULL)
			{
				uint32 curtable = calctable(i);
				if( curtable != lst)
					ret = add(ret,1);
			}

			lst = calctable(i);
			i = add(i,PAGE_SIZE);
		}
	return ret;
	//panic("calculate_required_frames() is not implemented yet...!!");
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size) {
    //TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
    // Write your code here, remove the panic and write your code
    //panic("free_user_mem() is not implemented yet...!!");
    uint32 *pt = NULL;
        uint32 start = ROUNDDOWN(virtual_address,PAGE_SIZE);
        uint32 end =  ROUNDUP(size+virtual_address,PAGE_SIZE);

        for (uint32 i = start; i < end; i += PAGE_SIZE)
            pf_remove_env_page(e, i);

        for (int i = 0; i < e->page_WS_max_size; i++) {

            if ((e ->ptr_pageWorkingSet[i].virtual_address>= start) && ( e ->ptr_pageWorkingSet[i].virtual_address < end)) {
            	uint32 use =  e ->ptr_pageWorkingSet[i].virtual_address;
            	env_page_ws_clear_entry(e, i);
                unmap_frame(e->env_page_directory, use);

            }
        }
        for (uint32 i = start; i < end; i+= PAGE_SIZE) {

            get_page_table(e->env_page_directory, i, &pt);
            if (pt != NULL){
            char c = 'y';
            for (int j = 0; j < 1024; j++) {
                if (pt[j] ==0)
                    continue;
                else
                {
                    c = 'n';
                    break;
                }
            }
            if (c == 'y')
            {
                kfree((void*) pt);
                pd_clear_page_dir_entry(e->env_page_directory,i);
            }
            }

        }
    }
//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

