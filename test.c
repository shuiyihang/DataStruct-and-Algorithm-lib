
#include "stdlib.h"
#include "stdio.h"

#include "./Common_Struct_lib/public.h"
#include "./Common_Struct_lib/Hi_Stack.h"

#include "./Common_Struct_lib/Hi_Queue.h"


#include "./Common_Struct_lib/Hi_list.h"

#include "./Sort_lib/Hi_sort.h"

#include "./Common_Struct_lib/Hi_PriorityQueue.h"

#include "./Graph_Algorithm_lib/Hi_Graph.h"
// typedef struct
// {
//     int data;
//     char* name;
//     struct stack_head clue;
//     struct queue_head queue_clue;
//     struct list_head list_clue;
// }test;

unsigned char test_maze[][10]={
    {0,0,0,1,0,0,0,0,1,1},
    {1,1,0,0,1,0,0,0,1,1},
    {0,0,0,1,0,0,0,0,0,0},
    {0,0,0,1,1,0,0,1,0,1},
    {0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,1,1,1,0,0},
    {0,0,0,0,0,0,1,1,1,0},
    {1,0,0,1,0,1,0,0,0,0},
    {1,0,1,0,0,0,0,1,0,0}
};





void main()
{

    struct win_size size={10,10};
    struct local_pos start,end;
    start.x=0;
    start.y=0;
    end.x=9;
    end.y=9;
    A_star(test_maze,size,start,end);
    // struct priority_queue_info info1,info2,info3,info4,info5;

    // info1.name="i am info1";
    // info1.prior=7;
    // info2.name="i am info2";
    // info2.prior=2;
    // info3.name="i am info3";
    // info3.prior=6;
    // info4.name="i am info4";
    // info4.prior=5;
    // info5.name="i am info5";
    // info5.prior=3;

    // LIST_HEAD(test_prior);
    // priority_queue_push(&info1,&test_prior);
    // priority_queue_push(&info2,&test_prior);
    // priority_queue_push(&info3,&test_prior);
    // priority_queue_push(&info4,&test_prior);
    // priority_queue_push(&info5,&test_prior);

    // struct priority_queue_info *pos=NULL;
    // list_for_each_entry(pos,&test_prior,prior_hook)
    // {
    //      printf("%s\n",pos->name);
    // }





    // test one;
    // test two;
    // test three;
    // test five;
    // test *four;
    // one.data=10;
    // one.name="one data";

    // two.data=20;
    // two.name="two data";

    // three.data=30;
    // three.name="three data";

    // five.data=50;
    // five.name="five data";

    // int data[]={15,29,10,20,40,30,11,29};


    // quick_sort(data,0,(sizeof(data)/sizeof(int))-1);
    // for(int i=0;i<sizeof(data)/sizeof(int);i++){
    //     printf("%d\n",data[i]);
    // }


    // LIST_HEAD(test_list);
    // list_add_head(&one.list_clue,&test_list);
    // list_add_head(&three.list_clue,&test_list);
    // list_add_head(&five.list_clue,&test_list);
    // list_add_head(&two.list_clue,&test_list);

    // test *pos=NULL;
    // list_for_each_entry(pos,&test_list,list_clue)
    // {
    //      printf("%d,%s\n",pos->data,pos->name);
    // }


    // STACK_HEAD(test_stack);
    // stack_push(&one.clue,&test_stack);
    // stack_push(&two.clue,&test_stack);

    // stack_push(&three.clue,&test_stack);
    // stack_push(&five.clue,&test_stack);

    // struct stack_head* p1;
    // while(!stack_empty(&test_stack))
    // {
    //     p1=stack_pop(&test_stack);
    //     four=CONTAINER_OF(p1,test,clue);
    //     printf("%d,%s\n",four->data,four->name);
    // }

    // QUEUE_HEAD(test_queue);
    // queue_push(&one.queue_clue,&test_queue);
    // queue_push(&two.queue_clue,&test_queue);
    // queue_push(&three.queue_clue,&test_queue);
    // queue_push(&five.queue_clue,&test_queue);

    // struct queue_head* p2;
    // while(!queue_empty(&test_queue))
    // {
    //     p2=queue_pop(&test_queue);
    //     four=queue_entry(p2,test,queue_clue);
    //     printf("%d,%s\n",four->data,four->name);
    // }


}