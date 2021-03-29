#ifndef __HI_QUEUE_H__
#define __HI_QUEUE_H__

#include "public.h"
typedef struct
{
    Hi_Seek_Ptr* front;
    Hi_Seek_Ptr* tail;
}Hi_Queue;


void Hi_initQueue(Hi_Queue* q);
void Hi_EnQueue(Hi_Queue* q,Hi_Seek_Ptr* node);
Hi_Seek_Ptr* Hi_DeQueue(Hi_Queue* q);
int Hi_Queue_is_empty(Hi_Queue q);



#endif