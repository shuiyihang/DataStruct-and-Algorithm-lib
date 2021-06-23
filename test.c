
#include "stdlib.h"
#include "stdio.h"

#include "./Common_Struct_lib/public.h"
#include "./Common_Struct_lib/Hi_Stack.h"

#include "./Common_Struct_lib/Hi_Queue.h"


#include "./Common_Struct_lib/Hi_dlist.h"

#include "./Sort_lib/Hi_sort.h"

#include "./Common_Struct_lib/Hi_PriorityQueue.h"

#include "./Graph_Algorithm_lib/Hi_Graph.h"

#include "./String_match_lib/kmp.h"

#include "./Common_Struct_lib/Hi_Queue_v2.h"


#define PRINT_MACRO_HELPER(x) #x
#define PRINT_MACRO(x) #x"="PRINT_MACRO_HELPER(x)           //全局宏定义查看调用


// #define COMMON_STRUCT_TEST





typedef struct
{
    int reserve;//保留
    int data;
    char* name;
    stack_head clue;
    queue_head queue_clue;
    struct single_list_head list_clue;
}test;

#ifdef ASTAR_TEST
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
#endif


#ifdef KMP_TEST
u8_t pat_str[]="ababcabaaa";
u8_t tar_str[]="abca";
#endif


#define adafruit_abs(a)         \
                ({((a)<0)?(-(a)):(a);})



void main()
{
    printf(">>>>%d\n",adafruit_abs(1-6));
#ifdef KMP_TEST
    int result=kmp(pat_str,sizeof(pat_str)-1,tar_str,sizeof(tar_str)-1);
    printf("result:%d\n",result);
#endif

#ifdef ASTAR_TEST
    struct win_size size={10,10};
    struct local_pos start,end;
    start.x=0;
    start.y=0;
    end.x=9;
    end.y=9;
    A_star(test_maze,size,start,end);
#endif

#ifdef PRI_QUEUE_TEST
    struct priority_queue_info info1,info2,info3,info4,info5;

    info1.name="i am info1";
    info1.prior=7;
    info2.name="i am info2";
    info2.prior=2;
    info3.name="i am info3";
    info3.prior=6;
    info4.name="i am info4";
    info4.prior=5;
    info5.name="i am info5";
    info5.prior=3;

    LIST_HEAD(test_prior);
    priority_queue_push(&info1,&test_prior);
    priority_queue_push(&info2,&test_prior);
    priority_queue_push(&info3,&test_prior);
    priority_queue_push(&info4,&test_prior);
    priority_queue_push(&info5,&test_prior);

    struct priority_queue_info *pos=NULL;
    list_for_each_entry(pos,&test_prior,prior_hook)
    {
         printf("%s\n",pos->name);
    }
#endif

#ifdef QUICK_SORT_TEST
    int data[]={15,29,10,20,40,30,11,29};
    quick_sort(data,0,(sizeof(data)/sizeof(int))-1);
    for(int i=0;i<sizeof(data)/sizeof(int);i++){
        printf("%d\n",data[i]);
    }
#endif


#ifdef COMMON_STRUCT_TEST
    test one;
    test two;
    test three;
    test five;
    test *four;
    one.data=10;
    one.name="one data";

    two.data=20;
    two.name="two data";

    three.data=30;
    three.name="three data";

    five.data=50;
    five.name="five data";

    
    SINGLE_LIST_HEAD(test_list);
    single_list_add_head(&one.list_clue,&test_list);
    single_list_add_head(&three.list_clue,&test_list);
    single_list_add_head(&five.list_clue,&test_list);
    single_list_add_head(&two.list_clue,&test_list);

    test *pos=NULL;
    single_list_for_each_entry(pos,&test_list,list_clue)
    {
         printf("list_test:%d,%s\n",pos->data,pos->name);
    }

    printf("stack start init\n");
    STACK_HEAD(test_stack);
    printf("stack init finish\n");
    stack_push(&one.clue,&test_stack);
    stack_push(&two.clue,&test_stack);

    stack_push(&three.clue,&test_stack);
    stack_push(&five.clue,&test_stack);
    printf("stack inset finish\n");
    stack_head* p1;
    while(!stack_empty(&test_stack))
    {
        p1=stack_pop(&test_stack);
        four=CONTAINER_OF(p1,test,clue);
        printf("stack_test:%d,%s\n",four->data,four->name);
    }

    QUEUE_HEAD(test_queue);
    queue_push(&one.queue_clue,&test_queue);
    queue_push(&two.queue_clue,&test_queue);
    queue_push(&three.queue_clue,&test_queue);
    queue_push(&five.queue_clue,&test_queue);

    queue_head* p2;
    while(!queue_empty(&test_queue))
    {
        p2=queue_pop(&test_queue);
        four=list_entry(p2,test,queue_clue);
        printf("queue_test:%d,%s\n",four->data,four->name);
    }
#endif

    test one, two, three;
    test *temp;
    one.name = "one test";
    two.name = "two test";
    three.name = "three test";

    QUEUE_HEAD_V2(queue);
    queue_append(&queue,&two);
    queue_append(&queue,&one);
    queue_append(&queue,&three);

    while(!queue_is_empty(&queue)){
        temp = queue_get(&queue);
        printf("queue_test:%s\n",temp->name);
    }
    

}