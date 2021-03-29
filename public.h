
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "stdlib.h"

#define CONTAINER_OF(ptr,type,member) ({\
const typeof(&((type*)0)->member) *__mptr=ptr;\
(type*)((char*)__mptr-(int)(&((type*)0)->member));\
})

/**
 * 需要使用栈的数据结构包含这个指针
*/
typedef struct node
{
    struct node* next;
}Hi_Seek_Ptr;










#endif 