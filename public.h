
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "stdlib.h"

#define CONTAINER_OF(ptr,type,member) ({\
const typeof(&((type*)0)->member) *__mptr=ptr;\
(type*)((char*)__mptr-(int)(&((type*)0)->member));\
})





#endif 