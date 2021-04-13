
/**
 * 同样的,队列则是链表的尾插法使用
*/

#ifndef __HI_QUEUE_H__
#define __HI_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "public.h"

struct queue_head
{
    struct queue_head *hook;
};
#define QUEUE_HEAD_INIT()       {NULL}
#define QUEUE_HEAD(name)            \
    struct queue_head name = QUEUE_HEAD_INIT()

static inline void queue_push(struct queue_head *_new,
                              struct queue_head *head)
{
    struct queue_head *temp = head;
    while(NULL != temp->hook){
        temp = temp->hook;
    }
    temp->hook = _new;
    _new->hook = NULL;
}

static inline struct queue_head* queue_pop(struct queue_head *head)
{
    struct queue_head* temp = head->hook;
    if(NULL == temp){
        printf("queue is null\n");
        return NULL;
    }
    head->hook = temp->hook;

    return temp;
}

static inline int queue_empty(const struct queue_head *head)
{
    return head->hook == NULL;
}




#define queue_entry(ptr, type, member)      \
    CONTAINER_OF(ptr, type, member)












#ifdef __cplusplus
}
#endif


#endif