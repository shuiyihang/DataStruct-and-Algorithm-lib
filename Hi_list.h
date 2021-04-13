
#ifndef __HI_LIST_H__
#define __HI_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"

struct list_head
{
    struct list_head *next,*prev;
};

//头节点的初始化
#define LIST_HEAD_INIT(name)    {&(name),&(name)}
#define LIST_HEAD(name)         \
    struct list_head name = LIST_HEAD_INIT(name)

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

static inline void __list_add(struct list_head *_new,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}


//尾插
static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head->prev, head);
}

static inline void list_add_head(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head, head->next);
}


static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
    
}
/**
 * 库里不做释放工作，如有需要外部定义指针变量malloc分配，free释放
*/
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}


#define list_entry(ptr, type, member)   \
    CONTAINER_OF(ptr, type, member)

//加括号是因为传进来的参数是一个取地址，防止出错
#define list_for_each_entry(pos, head, member)      \
    for(pos = list_entry((head)->next, typeof(*pos), member); \
        &pos->member != (head);                                   \
        pos = list_entry(pos->member.next, typeof(*pos), member))

#ifdef __cplusplus
}
#endif


#endif

