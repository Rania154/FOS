#include <inc/lib.h>

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

int FirstTimeFlag = 1;
void InitializeUHeap()
{
	if (FirstTimeFlag)
	{
		initialize_dyn_block_system();
		cprintf("DYNAMIC BLOCK SYSTEM IS INITIALIZED\n");
#if UHP_USE_BUDDY
		initialize_buddy();
		cprintf("BUDDY SYSTEM IS INITIALIZED\n");
#endif
		FirstTimeFlag = 0;
	}
}

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//=================================
void initialize_dyn_block_system()
{
	// TODO: [PROJECT MS3] [USER HEAP - USER SIDE] initialize_dyn_block_system
	//  your code is here, remove the panic and write your code
	// panic("initialize_dyn_block_system() is not implemented yet...!!");
	LIST_INIT(&FreeMemBlocksList);
	LIST_INIT(&AllocMemBlocksList);
	MAX_MEM_BLOCK_CNT = NUM_OF_UHEAP_PAGES;
	MemBlockNodes = (struct MemBlock *)USER_DYN_BLKS_ARRAY;
	sys_allocate_chunk(USER_DYN_BLKS_ARRAY, ROUNDUP((MAX_MEM_BLOCK_CNT * sizeof(struct MemBlock)), PAGE_SIZE), PERM_USER | PERM_WRITEABLE);
	initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);
	struct MemBlock *elmnt = LIST_LAST(&AvailableMemBlocksList);

	LIST_REMOVE(&AvailableMemBlocksList, elmnt);
	LIST_INSERT_TAIL(&FreeMemBlocksList, elmnt);

	elmnt->sva = USER_HEAP_START;
	uint32 sz = USER_HEAP_MAX - USER_HEAP_START;
	elmnt->size = sz;
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================

void *malloc(uint32 size)
{
	//==============================================================
	// DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0)
		return NULL;
	//==============================================================
	//==============================================================

	// TODO: [PROJECT MS3] [USER HEAP - USER SIDE] malloc
	struct MemBlock *block = NULL;

	if (sys_isUHeapPlacementStrategyFIRSTFIT())
	{
		block = alloc_block_FF(ROUNDUP(size, PAGE_SIZE));
		if (block == NULL)
			return NULL;
		else
		{
			insert_sorted_allocList(block);
			return ((void *)block->sva);
		}
	}
	else
		return NULL;
	// Use sys_isUHeapPlacementStrategyFIRSTFIT()... to check the current strategy
}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	FROM main memory AND free pages from page file then switch back to the user again.
//
//	We can use sys_free_user_mem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls free_user_mem() in
//		"kern/mem/chunk_operations.c", then switch back to the user mode here
//	the free_user_mem function is empty, make sure to implement it.
void free(void *virtual_address)
{
	// TODO: [PROJECT MS3] [USER HEAP - USER SIDE] free
	//  your code is here, remove the panic and write your code
	// panic("free() is not implemented yet...!!");
	struct MemBlock *elmnt = find_block(&AllocMemBlocksList, (uint32)virtual_address);
	if (elmnt != NULL)
	{
		sys_free_user_mem(elmnt->sva, elmnt->size);
		LIST_REMOVE(&AllocMemBlocksList, elmnt);
		insert_sorted_with_merge_freeList(elmnt);
	}
}

//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void *smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	// DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0)
		return NULL;
	//==============================================================

	// TODO: [PROJECT MS3] [SHARING - USER SIDE] smalloc()
	//  Write your code here, remove the panic and write your code
	// panic("smalloc() is not implemented yet...!!");
	//  Steps:
	//	1) Implement FIRST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	struct MemBlock *block = NULL;

	if (sys_isUHeapPlacementStrategyFIRSTFIT())
	{
		block = alloc_block_FF(ROUNDUP(size, PAGE_SIZE));
		if (block == NULL)
			return NULL;
		else
		{
			insert_sorted_allocList(block);

			if (sys_createSharedObject(sharedVarName, size, isWritable, (void *)block->sva) >= 0)
			{

				return ((void *)block->sva);
			}
			else
				return NULL;
		}
	}
	else
		return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void *sget(int32 ownerEnvID, char *sharedVarName)
{
	//==============================================================
	// DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================

	// TODO: [PROJECT MS3] [SHARING - USER SIDE] sget()
	if (sys_getSizeOfSharedObject(ownerEnvID, sharedVarName) == E_SHARED_MEM_NOT_EXISTS)
		return NULL;
	struct MemBlock *block = NULL;

	if (sys_isUHeapPlacementStrategyFIRSTFIT())
	{
		block = alloc_block_FF(ROUNDUP(sys_getSizeOfSharedObject(ownerEnvID, sharedVarName), PAGE_SIZE));
		if (block == NULL)
			return NULL;
		else
		{
			insert_sorted_allocList(block);

			if (sys_getSharedObject(ownerEnvID, sharedVarName, (void *)block->sva) >= 0)
			{

				return ((void *)block->sva);
			}
			else
				return NULL;
		}
	}
	else
		return NULL;
	//	4) if no suitable space found, return NULL
	//	 Else,
	//	5) Call sys_getSharedObject(...) to invoke the Kernel for sharing this variable
	//		sys_getSharedObject(): if succeed, it returns the ID of the shared variable. Else, it returns -ve
	//	6) If the Kernel successfully share the variable, return its virtual address
	//	   Else, return NULL
	//

	// This function should find the space for sharing the variable
	//  ******** ON 4KB BOUNDARY ******************* //

	// Use sys_isUHeapPlacementStrategyFIRSTFIT() to check the current strategy
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//==============================================================
	// DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================
	// [USER HEAP - USER SIDE] realloc
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
}

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void *virtual_address)
{
	// TODO: [PROJECT MS3 - BONUS] [SHARING - USER SIDE] sfree()

	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}

//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//
void expand(uint32 newSize)
{
	panic("Not Implemented");
}
void shrink(uint32 newSize)
{
	panic("Not Implemented");
}
void freeHeap(void *virtual_address)
{
	panic("Not Implemented");
}
