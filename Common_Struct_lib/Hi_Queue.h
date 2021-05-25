
/**
 * 单链表的队列实现
 * 同样的,队列则是链表的尾插法使用
*/

#ifndef __HI_QUEUE_H__
#define __HI_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "public.h"
#include "Hi_single_list.h"


typedef struct single_list_head    queue_head;

#define QUEUE_HEAD_INIT()       {NULL}
#define QUEUE_HEAD(name)            \
    queue_head name = QUEUE_HEAD_INIT()

/**
 * @brief 数据入队
 * @param _new      需要入队的新节点
 * @param head      队列的头节点
*/
static inline void queue_push(queue_head *_new,
                              queue_head *head)
{
    single_list_add_tail(_new,head);
}

/**
 * @brief           数据出队
 * @param head      队列的头节点
 * @return          返回出队数据的节点
*/
static inline queue_head* queue_pop(queue_head *head)
{
    queue_head* temp = head->next;
    if(NULL == temp){
        printf("queue is null\n");
        return NULL;
    }
    head->next = temp->next;

    return temp;
}

/**
 * @brief           队列判空
 * @param head      队列的头节点
 * @return          1:队列空 0:队列非空
*/
static inline int queue_empty(const queue_head *head)
{
    return head->next == NULL;
}




#ifdef __cplusplus
}
#endif


#endif