
#include "public.h"

#include "Hi_Stack.h"

void Hi_initStack(Hi_Stack* s)
{
    s->top=NULL;
}
void Hi_push(Hi_Stack* s,Hi_Seek_Ptr* node)
{
    node->next=s->top;
    s->top=node;
}
Hi_Seek_Ptr* Hi_pop(Hi_Stack* s)
{
    Hi_Seek_Ptr* ptr=s->top;
    s->top=ptr->next;
    return ptr;
}
int Hi_Stack_is_empty(Hi_Stack s)
{
    return s.top==NULL;
}












