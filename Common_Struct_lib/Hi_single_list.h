
#ifndef __HI_SINGLE_LIST_H__
#define __HI_SINGLE_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"


/**
 * 单链表
*/
struct single_list_head
{
    struct single_list_head *next;
};

//头节点的初始化
#define SINGLE_LIST_HEAD_INIT(name)    {NULL}
#define SINGLE_LIST_HEAD(name)         \
    struct single_list_head name = SINGLE_LIST_HEAD_INIT(name)

static inline int single_list_empty(const struct single_list_head *head)
{
    return head->next == NULL;
}

static inline void __single_list_add(struct single_list_head *_new,
                              struct single_list_head *prev,
                              struct single_list_head *next)
{
    //单链表，在某个节点之后插入
    _new->next = next;
    prev->next = _new;
}

//尾插
static inline void single_list_add_tail(struct single_list_head *_new, struct single_list_head *head)
{
    struct single_list_head *temp = head;
    while(NULL != temp->next){
        temp = temp->next;
    }
    __single_list_add(_new, temp , NULL);
}

static inline void single_list_add_head(struct single_list_head *_new, struct single_list_head *head)
{
    __single_list_add(_new, head, head->next);
}


static inline void __single_list_del(struct single_list_head *prev, struct single_list_head *next)
{
    prev->next = next;
}
/**
 * 库里不做内存释放工作，如有需要外部定义指针变量malloc分配，free释放
 * 参考A star算法的使用操作
*/
static inline void single_list_del(struct single_list_head *entry, struct single_list_head *head)
{
    
    struct single_list_head *temp = head;
    if(entry == NULL){
        return;
    }
    while(temp->next != entry && temp->next != NULL){
        temp = temp->next;
    }
    if(temp->next != entry){
        return;
    }
    __single_list_del(temp, entry->next);
}



#ifdef __cplusplus
}
#endif


#endif

