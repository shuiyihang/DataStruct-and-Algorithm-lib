
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "stdlib.h"

#define CONTAINER_OF(ptr,type,member) ({\
(type*)((char*)ptr-(size_t)(&((type*)0)->member));\
})


#define list_entry(ptr, type, member)   \
    CONTAINER_OF(ptr, type, member)

//加括号是因为传进来的参数是一个取地址，防止出错
#define list_for_each_entry(pos, head, member)      \
    for(pos = list_entry((head)->next, typeof(*pos), member); \
        &pos->member != (head);                                   \
        pos = list_entry(pos->member.next, typeof(*pos), member))


#endif 