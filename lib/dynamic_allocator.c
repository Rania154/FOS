/*
 * dyn_block_management.c
 *
 *  Created on: Sep 21, 2022
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//===========================
// PRINT MEM BLOCK LISTS:
//===========================

void print_mem_block_lists()
{
	cprintf("\n=========================================\n");
	struct MemBlock* blk ;
	struct MemBlock* lastBlk = NULL ;
	cprintf("\nFreeMemBlocksList:\n");
	uint8 sorted = 1 ;
	LIST_FOREACH(blk, &FreeMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nFreeMemBlocksList is NOT SORTED!!\n") ;

	lastBlk = NULL ;
	cprintf("\nAllocMemBlocksList:\n");
	sorted = 1 ;
	LIST_FOREACH(blk, &AllocMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nAllocMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\n=========================================\n");

}
int lastList = -1;
//********************************************************************************//
//********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//===============================
// [1] INITIALIZE AVAILABLE LIST:
//===============================
void initialize_MemBlocksList(uint32 numOfBlocks)
{
	// Write your code here, remove the panic and write your code
	LIST_INIT(&AvailableMemBlocksList);
	int i;
	for( i =0;i < numOfBlocks;i++)
	{
		LIST_INSERT_HEAD(&AvailableMemBlocksList, &(MemBlockNodes[i]));
	}
	//panic("initialize_MemBlocksList() is not implemented yet...!!");
}

//===============================
// [2] FIND BLOCK:
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{
	// Write your code here, remove the panic and write your code
	int i ;
	struct MemBlock *element = NULL;
	LIST_FOREACH(element, (blockList))
		{
			if ( (element->sva) == (va))
				return (element);
		}
	return NULL;
}

//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	// Write your code here, remove the panic and write your code
	struct MemBlock *element = NULL;
	lastList = blockToInsert->sva+blockToInsert->size;
	LIST_FOREACH(element, (&AllocMemBlocksList))
	{
		if ((element -> sva) > (blockToInsert->sva))
		{
			LIST_INSERT_BEFORE(	(&AllocMemBlocksList) ,element, blockToInsert);
			return;
		}
	}
	LIST_INSERT_TAIL(&AllocMemBlocksList, blockToInsert);
	return;
	//panic("insert_sorted_allocList() is not implemented yet...!!");
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{
	// Write your code here, remove the panic and write your code
	struct MemBlock *element = NULL;
	LIST_FOREACH(element, (&FreeMemBlocksList))
		{
			if ( (element->size) >= (size))
			{
				if ( element->size == size)
				{
					LIST_REMOVE ((&FreeMemBlocksList) , element);
					return element;
				}
				else
				{
					struct MemBlock *it = AvailableMemBlocksList.lh_first;
					LIST_REMOVE ((&AvailableMemBlocksList) , it);
					it->prev_next_info = element ->prev_next_info;
					it -> size = size;
					it -> sva = element ->sva;
					element-> sva += size;
					element -> size -= size;
					return  it ;
				}
			}
		}
		return NULL;
	//panic("alloc_block_FF() is not implemented yet...!!");
}

//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{
	// Write your code here, remove the panic and write your code
	struct MemBlock *element = NULL;
	uint32  sz = 0;
	LIST_FOREACH(element, (&FreeMemBlocksList))
	{
		if ( (element->size) >= (size))
		{
			if ( sz > element->size || (sz == 0))
				sz = element->size;
		}
	}
	if ( sz == 0)
		return NULL;
	element = NULL;
	LIST_FOREACH(element, (&FreeMemBlocksList))
	{
		if ( (element->size) == sz)
		{
			if ( element->size == size)
			{
				LIST_REMOVE ((&FreeMemBlocksList) , element);
				return element;
			}
			else
			{
				struct MemBlock *it = AvailableMemBlocksList.lh_first;
				LIST_REMOVE ((&AvailableMemBlocksList) , it);
				it->prev_next_info = element ->prev_next_info;
				it -> size = size;
				it -> sva = element ->sva;
				element-> sva += size;
				element -> size -= size;
				return  it ;
			}
		}
	}
	return NULL;
}
//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
struct MemBlock *alloc_block_NF(uint32 size)
{
	// Write your code here, remove the panic and write your code
		struct MemBlock *element = NULL;
		LIST_FOREACH(element, (&FreeMemBlocksList))
					{
						if ( (element->size) >= (size) && (lastList == -1  || element->sva >= lastList))
						{
							if ( element->size == size)
							{
								LIST_REMOVE ((&FreeMemBlocksList) , element);
								lastList = element->sva+element->size;
								return element;
							}
							else
							{
								struct MemBlock *it = AvailableMemBlocksList.lh_first;
								LIST_REMOVE ((&AvailableMemBlocksList) , it);
								it->prev_next_info = element ->prev_next_info;
								it -> size = size;
								it -> sva = element ->sva;
								element-> sva += size;
								element -> size -= size;
								lastList = it->sva+it->size;
								return  it ;
							}
						}
					}
		element = NULL;
		LIST_FOREACH(element, (&FreeMemBlocksList))
			{
				if ( (element->size) >= (size))
				{
					if ( element->size == size)
					{
						LIST_REMOVE ((&FreeMemBlocksList) , element);
						lastList = element->sva+element->size;
						return element;
					}
					else
					{
						struct MemBlock *it = AvailableMemBlocksList.lh_first;
						LIST_REMOVE ((&AvailableMemBlocksList) , it);
						it->prev_next_info = element ->prev_next_info;
						it -> size = size;
						it -> sva = element ->sva;
						element-> sva += size;
						element -> size -= size;
						lastList = it->sva+it->size;
						return  it ;
					}
				}
			}
			return NULL;
	//panic("alloc_block_NF() is not implemented yet...!!");

}

//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{
	//cprintf("BEFORE INSERT with MERGE: insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
	//print_mem_block_lists() ;

	// Write your code here, remove the panic and write your code
	//panic("insert_sorted_with_merge_freeList() is not implemented yet...!!");

	struct MemBlock *element = NULL;
	bool flag  = 0;
		LIST_FOREACH(element, (&FreeMemBlocksList))
		{
			if (((element -> sva) > (blockToInsert->sva)) && flag == 0)
			{
				LIST_INSERT_BEFORE(	(&FreeMemBlocksList) ,element, blockToInsert);
				flag = 1;
				break;
			}
		}
	if ( flag == 0)
		LIST_INSERT_TAIL(&FreeMemBlocksList, blockToInsert);
	element = NULL;
	flag= 1;
	int lst =0;
	while ( flag)
	{
		flag = 0;
		lst =0;
		struct MemBlock *pre = NULL;
		struct MemBlock *temp = NULL;
		element = NULL;
		LIST_FOREACH(element, (&FreeMemBlocksList))
		{
			if ((element-> sva == lst) && lst != 0)
			{
				temp = element;
				pre = element->prev_next_info.le_prev;
				pre->size += element->size;
				LIST_REMOVE ((&FreeMemBlocksList) , element);
				temp->size = 0;
				temp->sva = 0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList,temp);
				flag = 1;
				break;
			}
			lst = element->sva+element->size;
		}
	}
	//cprintf("\nAFTER INSERT with MERGE:\n=====================\n");
	//print_mem_block_lists();

}

