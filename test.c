
#include "stdlib.h"
#include "stdio.h"

#include "public.h"
#include "Hi_Stack.h"

#include "Hi_Queue.h"
typedef struct
{
    int data;
    char* name;
    Hi_Seek_Ptr clue;
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

    Hi_Stack S1;
    Hi_initStack(&S1);

    Hi_push(&S1,&two.clue);
    Hi_push(&S1,&one.clue);
    Hi_push(&S1,&three.clue);
    


    Hi_Queue Q1;
    Hi_initQueue(&Q1);

    Hi_EnQueue(&Q1,&three.clue);
    Hi_EnQueue(&Q1,&two.clue);
    Hi_EnQueue(&Q1,&one.clue);
    Hi_EnQueue(&Q1,&five.clue);









    Hi_Seek_Ptr* p1;
    // while(!Hi_Stack_is_empty(S1))
    // {
    //     p1=Hi_pop(&S1);
    //     four=CONTAINER_OF(p1,test,clue);
    //     printf("%d,%s\n",four->data,four->name);
    // }


    //  printf("======================\n");


    while(!Hi_Queue_is_empty(Q1))
    {
        p1=Hi_DeQueue(&Q1);
        four=CONTAINER_OF(p1,test,clue);
        printf("%d,%s\n",four->data,four->name);
    }

}