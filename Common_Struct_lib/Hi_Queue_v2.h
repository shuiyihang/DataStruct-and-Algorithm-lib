/**
 * 增加一个尾指针来快速的存储数据
*/
#ifndef __HI_QUEUE_V2_H__
#define __HI_QUEUE_V2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "Hi_slist.h"


typedef struct single_list_head sys_snode_t;

typedef struct {
	sys_snode_t *head;
	sys_snode_t *tail;
}Hi_Queue_v2;



#define QUEUE_HEAD_V2_INIT()       {NULL,NULL}
#define QUEUE_HEAD_V2(name)            \
    Hi_Queue_v2 name = QUEUE_HEAD_INIT()



//放入的数据第一位可以使指针/int，只要四个字节即可，其余是队列中的使用数据
static inline void __queue_insert(Hi_Queue_v2 *list,
                                    sys_snode_t *prev,
                                    sys_snode_t *node)
{
    node->next = NULL;
    if(!prev){
        list->tail = node;
        list->head = node;
    }else{
        list->tail->next = node;
        list->tail = node;
    }
}

void queue_append(Hi_Queue_v2 *queue, void *data)
{
	return __queue_insert(queue, queue->tail, data);
}



static inline u8_t queue_is_empty(Hi_Queue_v2 *list)
{
	return (!list->head);
}


static inline sys_snode_t *__get_member_addr(Hi_Queue_v2 *queue)
{
	sys_snode_t *node = queue->head;

	queue->head = node->next;

	if (queue->tail == node) {
		queue->tail = queue->head;
	}

	return node;
}


void *queue_get(Hi_Queue_v2 *queue)
{
    void *data;
    if (queue_is_empty(queue)) {
        return NULL;
	}else{
        data = __get_member_addr(queue);
		return data;
    }
}

#ifdef __cplusplus
}
#endif


#endif