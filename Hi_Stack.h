
/**
 * 链表是一种数据结构的组织形式
 * 而栈和队列是可以使用这种组织形式的服从一定规则的数据结构
 * 其实下面的用法不正是头插法,为了节省数据,只使用单向链表
*/


#ifndef __HI_STACK_H__
#define __HI_STACK_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "public.h"


struct stack_head
{
    struct stack_head *hook;
};
#define STACK_HEAD_INIT()       {NULL}
#define STACK_HEAD(name)            \
    struct stack_head name = STACK_HEAD_INIT()

static inline void stack_push(struct stack_head *_new,
                              struct stack_head *top)
{
    _new->hook = top->hook;
    top->hook = _new;
}

static inline struct stack_head* stack_pop(struct stack_head *top)
{
    struct stack_head* temp = top->hook;
    if(NULL == temp){
        printf("stack is null\n");
        return NULL;
    }
    top->hook = temp->hook;
    return temp;
}

static inline int stack_empty(const struct stack_head *top)
{
    return top->hook == NULL;
}


#define stack_entry(ptr, type, member)      \
    CONTAINER_OF(ptr, type, member)




#ifdef __cplusplus
}
#endif


#endif