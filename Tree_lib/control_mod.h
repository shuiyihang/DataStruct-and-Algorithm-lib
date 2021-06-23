#ifndef __CONTROL_MOD_H__
#define __CONTROL_MOD_H__
//搞一些控件
#include "../Common_Struct_lib/Hi_slist.h"
#include "../Common_Struct_lib/public.h"

typedef struct 
{
    struct single_list_head page_start;
}page_info;

typedef struct 
{
    struct single_list_head node;
    unsigned char dny_or_static;//控件是动态还是静态的
    unsigned char mod_type;//控件类型
}module_info;

typedef struct
{
    unsigned char x;
    unsigned char y;
    unsigned char size;
    char *text;
    module_info info;
    
}button;

//滚动条
typedef struct 
{
    unsigned char x;
    unsigned char y;
    module_info info;
}scrollbar;

//圆环控件
typedef struct 
{
    unsigned char x;
    unsigned char y;
    unsigned char tar;//指示是电量还是音量之类
    unsigned char size;
    module_info info;
}ring;

//数码管显示的时间
typedef struct 
{
    unsigned char x;
    unsigned char y;
    module_info info;
}time_tube;


enum{
    BUTTON_MOD,
    SCROLL_MOD,
    RING_MOD,
    TIME_TUBE_MOD,
};


#endif