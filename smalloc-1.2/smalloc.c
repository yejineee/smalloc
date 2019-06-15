#include <unistd.h>
#include <stdio.h>
#include "smalloc.h"

sm_container_ptr sm_first = 0x0 ;
sm_container_ptr sm_last = 0x0 ;
sm_container_ptr sm_unused_containers = 0x0 ;


void sm_container_split(sm_container_ptr hole, size_t size)
{
	sm_container_ptr itr = 0x0 ;
	sm_container_ptr remainder = hole->data + size ;

	remainder->data = ((void *)remainder) + sizeof(sm_container_t) ;
	remainder->dsize = hole->dsize - size - sizeof(sm_container_t) ;
	remainder->status = Unused ;
	remainder->next = hole->next ;
	hole->next = remainder ;

	if (hole == sm_last)
		sm_last = remainder ;

	if (sm_unused_containers == 0x0){
		sm_unused_containers = remainder ;
		remainder->next_unused = 0x0 ;
		return ;
	}

	if( sm_unused_containers == hole){
		remainder->next_unused = sm_unused_containers->next_unused ;
		sm_unused_containers = remainder ;
		return ;
	}

	for (itr = sm_unused_containers ; itr->next_unused != 0x0 ; itr = itr->next_unused){
		if(itr->next_unused == hole){
			itr->next_unused = remainder ;
			remainder->next_unused = hole->next_unused ;
			hole->next_unused = 0x0 ;
			return ;
		}
	}
	itr->next_unused = remainder ; //새로 할당된 hole

}

void * sm_retain_more_memory(int size)
{
	sm_container_ptr hole ;
	int pagesize = getpagesize() ;
	int n_pages = 0 ;

	n_pages = (sizeof(sm_container_t) + size + sizeof(sm_container_t)) / pagesize  + 1 ;
	hole = (sm_container_ptr) sbrk(n_pages * pagesize) ;
	if (hole == 0x0)
		return 0x0 ;

	hole->data = ((void *) hole) + sizeof(sm_container_t) ;
	hole->dsize = n_pages * getpagesize() - sizeof(sm_container_t) ;
	hole->status = Unused ;
	return hole ;
}

void * smalloc(size_t size)
{
	sm_container_ptr hole = 0x0 ;
	size_t min_frag = 0;
	sm_container_ptr itr = 0x0 ;
	for (itr = sm_unused_containers ; itr != 0x0 ; itr = itr->next_unused) {


		if (size == itr->dsize) {
			// a hole of the exact size
			itr->status = Busy ;
			return itr->data ;
		}
		else if (size + sizeof(sm_container_t) < itr->dsize) {
			// a hole large enough to split (bestfit)
			if(min_frag == 0){
				min_frag = itr->dsize - (size + sizeof(sm_container_t)) ;
				hole = itr ;
			}
			else if(itr->dsize - (size + sizeof(sm_container_t)) < min_frag){
				min_frag = itr->dsize - (size + sizeof(sm_container_t)) ;
				hole = itr ;
			}

		}
	}
	if (hole == 0x0) {
		hole = sm_retain_more_memory(size) ;

		if (hole == 0x0)
			return 0x0 ;

		if (sm_first == 0x0) {
			sm_first = hole ;
			sm_last = hole ;
			hole->next = 0x0 ;
		}
		else {
			sm_last->next = hole ;
			sm_last = hole ;
			hole->next = 0x0 ;
		}
	}
	sm_container_split(hole, size) ;
	hole->dsize = size ;
	hole->status = Busy ;
	return hole->data ;
}

void sfree(void * p)
{
	sm_container_ptr itr ,target, prev, start, end ;
	size_t size_sum = 0;
	prev = 0x0;
	for (itr = sm_first ; itr->next != 0x0 ; itr = itr->next) {
		if (itr->data == p) {
			itr->status = Unused ;
			target = itr;
			break ;
		}
		prev = itr ;
	}
	size_sum += target->dsize ;
		
	start = target ; 	

	if(prev != 0x0 && prev->status == Unused){
		start = prev;
		size_sum += prev->dsize + sizeof(sm_container_t) ;
	}

	end = target ;

	if(end->next->status == Unused){
		end = end->next ;
		size_sum += end->dsize + sizeof(sm_container_t) ;
		start-> next_unused = end->next_unused ;
	}
	
	if( end == sm_last ){
		sm_last = start ;
	}
	if( end == sm_unused_containers ){
		sm_unused_containers = start ;
	}
	start-> next = end-> next ;
	start-> dsize = size_sum ;
}

void print_sm_containers()
{
	sm_container_ptr itr ;
	int i = 0 ;

	printf("==================== sm_containers ====================\n") ;
	for (itr = sm_first ; itr != 0x0 ; itr = itr->next, i++) {
		char * s ;
		printf("%3d:%p:%s:", i, itr->data, itr->status == Unused ? "Unused" : "  Busy") ;
		printf("%8d:", (int) itr->dsize) ;

		for (s = (char *) itr->data ;
			 s < (char *) itr->data + (itr->dsize > 8 ? 8 : itr->dsize) ;
			 s++)
			printf("%02x ", *s) ;
		printf("\n") ;
	}
printf("=======================================================\n") ;

}
void print_sm_uses()
{
	int total_mem = 0;
	int unused_mem = 0;
	int busy_mem = 0;
	sm_container_ptr itr ;
	int i = 0 ;
	for (itr = sm_first ; itr != 0x0 ; itr = itr->next, i++) {
//		total_mem += (int) itr->dsize ;
		total_mem += (int) itr->dsize + (int)sizeof(sm_container_t);
		if(itr->status == Unused){
			unused_mem += (int) itr->dsize ;
		}
		else{
			busy_mem += (int) itr->dsize ;
		}
	}
	fprintf(stderr, "the amount of memory retained by smalloc so far :%d\n",total_mem) ;
	fprintf(stderr, "the amount of memory allocated by smalloc at this moment :%d\n",busy_mem) ;
	fprintf(stderr, "the amount of memory retained by smalloc but not currently allocated :%d\n",unused_mem) ;

}

