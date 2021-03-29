#ifndef __HI_STACK_H__
#define __HI_STACK_H__

#include "public.h"
typedef struct
{
    Hi_Seek_Ptr* top;//只维护一个栈顶指针
}Hi_Stack;


void Hi_initStack(Hi_Stack* s);
Hi_Seek_Ptr* Hi_pop(Hi_Stack* s);
int Hi_Stack_is_empty(Hi_Stack s);
#endif