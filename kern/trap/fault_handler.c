/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE) {
	assert(
			LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE;
}
void setPageReplacmentAlgorithmCLOCK() {
	_PageRepAlgoType = PG_REP_CLOCK;
}
void setPageReplacmentAlgorithmFIFO() {
	_PageRepAlgoType = PG_REP_FIFO;
}
void setPageReplacmentAlgorithmModifiedCLOCK() {
	_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;
}
/*2018*/void setPageReplacmentAlgorithmDynamicLocal() {
	_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;
}
/*2021*/void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps) {
	_PageRepAlgoType = PG_REP_NchanceCLOCK;
	page_WS_max_sweeps = PageWSMaxSweeps;
}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE) {
	return _PageRepAlgoType == LRU_TYPE ? 1 : 0;
}
uint32 isPageReplacmentAlgorithmCLOCK() {
	if (_PageRepAlgoType == PG_REP_CLOCK)
		return 1;
	return 0;
}
uint32 isPageReplacmentAlgorithmFIFO() {
	if (_PageRepAlgoType == PG_REP_FIFO)
		return 1;
	return 0;
}
uint32 isPageReplacmentAlgorithmModifiedCLOCK() {
	if (_PageRepAlgoType == PG_REP_MODIFIEDCLOCK)
		return 1;
	return 0;
}
/*2018*/uint32 isPageReplacmentAlgorithmDynamicLocal() {
	if (_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL)
		return 1;
	return 0;
}
/*2021*/uint32 isPageReplacmentAlgorithmNchanceCLOCK() {
	if (_PageRepAlgoType == PG_REP_NchanceCLOCK)
		return 1;
	return 0;
}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt) {
	_EnableModifiedBuffer = enableIt;
}
uint8 isModifiedBufferEnabled() {
	return _EnableModifiedBuffer;
}

void enableBuffering(uint32 enableIt) {
	_EnableBuffering = enableIt;
}
uint8 isBufferingEnabled() {
	return _EnableBuffering;
}

void setModifiedBufferLength(uint32 length) {
	_ModifiedBufferLength = length;
}
uint32 getModifiedBufferLength() {
	return _ModifiedBufferLength;
}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va) {
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory,
				(uint32) fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}
void page_fault_handler(struct Env * curenv, uint32 fault_va) {
	//TODO: [PROJECT MS3] [FAULT HANDLER] page_fault_handler
	// Write your code here, remove the panic and write your code

	if (curenv->page_WS_max_size == env_page_ws_get_size(curenv)) {
			uint32 virtual, *pt = NULL;
			struct FrameInfo *fifo = NULL;
		while (1) {
			virtual = curenv->ptr_pageWorkingSet[curenv->page_last_WS_index].virtual_address;
			if ((pt_get_page_permissions(curenv->env_page_directory, virtual)& PERM_USED)) {
				pt_set_page_permissions(curenv->env_page_directory, virtual, 0,PERM_USED);
			}
			else
			{
				fifo = get_frame_info(curenv->env_page_directory, virtual, &pt);
				if ((pt_get_page_permissions(curenv->env_page_directory, virtual) & PERM_MODIFIED)) {
					pf_update_env_page(curenv, virtual, fifo);
				}
				unmap_frame(curenv->env_page_directory, virtual);
				env_page_ws_clear_entry(curenv,curenv->page_last_WS_index);
				break;
			}
			curenv->page_last_WS_index++;
			curenv->page_last_WS_index%=curenv->page_WS_max_size;
		}
		allocate_frame(&fifo);
		map_frame(curenv->env_page_directory, fifo, fault_va,PERM_PRESENT | PERM_USER | PERM_WRITEABLE);
		if (pf_read_env_page(curenv, (void *) fault_va) == E_PAGE_NOT_EXIST_IN_PF) {
			int a = (fault_va < USTACKTOP && fault_va >= USTACKBOTTOM);
						int b = (fault_va < USER_HEAP_MAX && fault_va >= USER_HEAP_START);
						if ( a == b && b==0 )
							panic("ILLEGAL MEMORY ACCESS");
		}
		while(curenv ->ptr_pageWorkingSet[curenv->page_last_WS_index].empty == 0)
				{
					curenv->page_last_WS_index++;
						(curenv->page_last_WS_index) %=curenv->page_WS_max_size;
				}

				env_page_ws_set_entry(curenv, curenv->page_last_WS_index, fault_va);
				curenv->page_last_WS_index++;
					(curenv->page_last_WS_index) %=curenv->page_WS_max_size;


	}
	else
	{
			uint32 virtual, *pt = NULL;
			struct FrameInfo *fifo = NULL;
	allocate_frame(&fifo);
	map_frame(curenv->env_page_directory, fifo, fault_va,PERM_PRESENT| PERM_USER | PERM_WRITEABLE *1);
	if (pf_read_env_page(curenv, (void *) fault_va) == E_PAGE_NOT_EXIST_IN_PF) {
		int a = (fault_va < USTACKTOP && fault_va >= USTACKBOTTOM);
					int b = (fault_va < USER_HEAP_MAX && fault_va >= USER_HEAP_START);
					if ( a == b && b==0 )
						panic("ILLEGAL MEMORY ACCESS");
	}

	while(curenv ->ptr_pageWorkingSet[curenv->page_last_WS_index].empty == 0)
			{
				curenv->page_last_WS_index++;
					(curenv->page_last_WS_index) %=curenv->page_WS_max_size;
			}

			env_page_ws_set_entry(curenv, curenv->page_last_WS_index, fault_va);
			curenv->page_last_WS_index++;
				(curenv->page_last_WS_index) %=curenv->page_WS_max_size;
	}

}
void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va) {
	// Write your code here, remove the panic and write your code
	panic("__page_fault_handler_with_buffering() is not implemented yet...!!");

}
