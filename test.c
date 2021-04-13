
#include "stdlib.h"
#include "stdio.h"

#include "public.h"
#include "Hi_Stack.h"

#include "Hi_Queue.h"


#include "Hi_list.h"
typedef struct
{
    int data;
    char* name;
    struct stack_head clue;
    struct queue_head queue_clue;
    struct list_head list_clue;
}test;

void main()
{
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

    QUEUE_HEAD(test_queue);
    queue_push(&one.queue_clue,&test_queue);
    queue_push(&two.queue_clue,&test_queue);
    queue_push(&three.queue_clue,&test_queue);
    queue_push(&five.queue_clue,&test_queue);

    struct queue_head* p2;
    while(!queue_empty(&test_queue))
    {
        p2=queue_pop(&test_queue);
        four=queue_entry(p2,test,queue_clue);
        printf("%d,%s\n",four->data,four->name);
    }


}