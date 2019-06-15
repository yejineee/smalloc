#include <stdio.h>
#include "smalloc.h"

int 
main()
{
	void *p1, *p2, *p3, *p4 ;

	print_sm_containers() ;

	p1 = smalloc(2000) ; 
	printf("smalloc(2000)\n") ; 
	print_sm_containers() ;
//	print_unused_linkedlist() ; 
	p2 = smalloc(2500) ; 
	printf("smalloc(2500)\n") ; 
	print_sm_containers() ;
//	print_unused_linkedlist();
	
	p3 = smalloc(1000) ; 
	printf("smalloc(1000)\n") ; 
	print_sm_containers() ;

//	print_unused_linkedlist();
	p4 = smalloc(950) ; 
	printf("smalloc(950)\n") ; 
	print_sm_containers() ;
	void * p5;
	p5 = smalloc(980) ; 
	printf("smalloc(980)\n") ; 
	print_sm_containers() ;


//	print_unused_linkedlist();

/*
	void *p5, *p6;	
	p6 = smalloc(2500) ; 
	printf("smalloc(2500)\n") ; 
	print_sm_containers() ;

	print_unused_linkedlist();

	p5 = smalloc(2400);
	printf("smalloc(2400)\n");
	print_sm_containers() ; 

	print_unused_linkedlist();
*/
	sfree(p1);
	printf("sfree(%p)\n",p1);
	print_sm_containers() ; 
	
//	print_unused_linkedlist();

/*
	sfree(p4);
	printf("sfree(%p)\n",p4);
	print_sm_containers() ; 
	
	print_unused_linkedlist();

	sfree(p5);
	printf("sfree(%p)\n",p5);
	print_sm_containers() ; 
		
	print_unused_linkedlist();

	void *q, *e;
	q = smalloc(2460);
	printf("smalloc(2460)\n");
	print_sm_containers() ; 

	print_unused_linkedlist();
*/

	print_sm_uses() ;
}

