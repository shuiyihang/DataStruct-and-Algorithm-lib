#include "public.h"
#include "Hi_Queue.h"


void Hi_initQueue(Hi_Queue* q)
{
    // q->front->next=NULL;
    // q->tail->next=NULL;
    q->tail=NULL;
}

void Hi_EnQueue(Hi_Queue* q,Hi_Seek_Ptr* node)
{
    q->tail->next=node;
    q->tail=node;
}
Hi_Seek_Ptr* Hi_DeQueue(Hi_Queue* q)
{
    Hi_Seek_Ptr* temp;
    if(q->front!=q->tail)
    {
        temp=q->front->next;

    }else
    {
        return NULL;
    }
}

int Hi_Queue_is_empty(Hi_Queue q)
{
    return q.front==q.tail;
}