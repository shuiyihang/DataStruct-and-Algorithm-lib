
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "stdlib.h"

#define CONTAINER_OF(ptr,type,member) ({\
(type*)((char*)ptr-(size_t)(&((type*)0)->member));\
})


#endif 