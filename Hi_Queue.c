#include "public.h"
#include "Hi_Queue.h"


void Hi_initQueue(Hi_Queue* q)
{
    q->front=q->tail=NULL;
}

void Hi_EnQueue(Hi_Queue* q,Hi_Seek_Ptr* node)
{
    if(!q->tail)
    {
        q->front=q->tail=node;
        q->tail->next=NULL;
        return;
    }
    q->tail->next=node;
    q->tail=node;
    q->tail->next=NULL;
}
Hi_Seek_Ptr* Hi_DeQueue(Hi_Queue* q)
{
    Hi_Seek_Ptr* temp;
    if(q->front!=NULL)
    {
        temp=q->front;
        q->front=temp->next;
        return temp;

    }else
    {
        return NULL;
    }
}

int Hi_Queue_is_empty(Hi_Queue q)
{
    return q.front==NULL;
}