#ifndef __MENUBASE_H__
#define __MENUBASE_H__

#include "string.h"
#include "stdarg.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "../../Common_Struct_lib/Hi_single_list.h"


#define True    1
#define False   0

#define PAGE_NUMS   4   //一页最多容纳 3 行

#define BUFF_NUMS   10

typedef struct MenuItem MenuItem_Typedef;

typedef void (*NodeBindingCb)( MenuItem_Typedef *);


typedef struct keybuff
{
    u8_t keycode[BUFF_NUMS];//键值
    u8_t write;
    u8_t read;
}keybuff_Typedef;


typedef void (*nonParamCb)(void);
typedef void (*keyCb)(keybuff_Typedef *buff);

/**
 * TODO 
 * 简化处理函数的类型
 * 
*/
typedef struct iconInfo
{
    const char *on_icon;
    const char *off_icon;
}iconInfo_Typedef;
typedef struct MenuItem
{
    u16_t unitType;          //该节点的类型

    /*切到下一级时,记录本地用户选择信息*/
    s8_t selectNum;         //选中的条目序号
    s8_t cursorPos;         //光标位置

    const char *briefInfo;  //节点内容信息

    //根据具体情况决定这一块的去留,非核心
    const iconInfo_Typedef *icon;       //菜单的图标信息
    const char *cur_icon;

    struct single_list_head  localPos;  //绑定子目录的头节点
    struct single_list_head  brother;
    struct single_list_head *parentPtr;


    nonParamCb setup;
    nonParamCb loop;
    nonParamCb exit;
    keyCb keyEventProgress;

    u8_t id;



    /*   用于一些特殊需求来调参数
     *   统一成指针可以索引一个结构体数据，理论上就可以管理用户需要的许多数据
    */
    void *param;
    
}MenuItem_Typedef;




#define KEY_RESERVED    '0'
#define KEY_UP          'w'
#define KEY_DOWN        's'
#define KEY_BACK        'a'
#define KEY_ENTER       'd'
#define KEY_EDIT        'e'





#define LEAF_TYPE_BIT   (14)
#define LEAF_MUTLI_BIT  (13)
#define LEAF_STATE_BIT  (12)

#define CAN_ENTER_ASSERT    (3 << LEAF_TYPE_BIT)
#define LEAF_INIT_STATE     (1 << LEAF_STATE_BIT)
#define LEAF_MULTI_ASSERT   (1 << LEAF_MUTLI_BIT)

#define NEED_DYN_ASSERT     (1)


/**
 * 位值代表
 * 
 * 低八位:
 * 0叶子/1非叶子  1展开/0不能展开    1多选/0单选            默认状态1开/0关
 * null,         null,            1可以编辑/0不能编辑     1需要动态刷新/0静态显示的
*/


enum UNIT_BIT_STATUS {
    UNIT_NONE = 0x0,
    UNIT_NON_LEAF = 0x8000,
    UNIT_UNFOLD = 0x4000,
    UNIT_MULTI_EN = 0x2000,
    UNIT_SWITCH_ON = 0x1000,
    UNIT_EDIT_EN = 0x0800,
    UNIT_REFRESH_EN = 0x0400,
};

typedef enum {
    NON_LEAF = UNIT_NON_LEAF,//非叶子节点

    NON_LEAF_EDIT = UNIT_NON_LEAF|UNIT_EDIT_EN,//非叶子节点可编辑页面

    NON_LEAF_MULTI_EDIT = UNIT_NON_LEAF|UNIT_EDIT_EN|UNIT_MULTI_EN,

    LEAF_OPEN_STATIC = UNIT_UNFOLD ,//可以展开的叶子节点,静态显示

    LEAF_OPEN_REFRESH = UNIT_UNFOLD|UNIT_REFRESH_EN,//需要动态刷新的可以展开的叶子节点

    LEAF_OPEN_EDIT_REFRESH = UNIT_UNFOLD|UNIT_EDIT_EN|UNIT_REFRESH_EN,//可以进行编辑可以展开需要实时刷新的叶子节点

    LEAF_CLOSE_MULTI_OFF = UNIT_MULTI_EN,//不能展开的叶子节点, 支持多选, 默认状态是关

    LEAF_CLOSE_MULTI_ON = UNIT_MULTI_EN|UNIT_SWITCH_ON,//不能展开的叶子节点, 支持多选, 默认状态是开

    LEAF_CLOSE_OFF = UNIT_NONE,//不能展开的叶子节点, 不支持多选, 默认状态是关

    LEAF_CLOSE_ON   = UNIT_SWITCH_ON,//不能展开的叶子节点, 不支持多选, 默认状态是开

    LEAF_CLOSE_EDIT = UNIT_EDIT_EN|UNIT_REFRESH_EN,
}NODE_TYPE;

enum{
    CLOSE_LEAF_SIGN = 0,//也即原来的叶子节点,如果是这种节点,不要进入下一页
    OPEN_LEAF_SIGN = 1,
    NON_LEAF_SIGN = 2,
};

enum{
    ENTER_PAGE,
    RETURN_PAGE,
};

typedef struct curHandle
{
    u16_t cur_type;
    u8_t chosse_cnt;
    u8_t need_refresh;//是否需要刷屏

    u8_t show_cnt;

    s8_t cur_choose;
    s8_t startItem;//顶叶序号
    s8_t cursorPos;//光标位置
    
}curHandle_Typedef;



static inline u8_t __get_node_type(u16_t type)
{
    return (type>>LEAF_TYPE_BIT)&3;
};

static inline u8_t __node_edit_assert(u16_t edit)
{
    return (edit>>8)&8;
}

static inline u8_t __node_multi_assert(u16_t multi)
{
    return (multi>>12)&2;
}






/*为了实现编辑模式下时间的转发定义按键函数*/
void keybuffInit(keybuff_Typedef *buff);

u8_t keybuffIsEmpty(keybuff_Typedef *buff);

u8_t keybuffIsFull(keybuff_Typedef *buff);

u8_t getKeyFromBuff(keybuff_Typedef *buff);

void putKeyToBuff(keybuff_Typedef *buff , u8_t key);





void bindParamInit(MenuItem_Typedef* node, void *bindParam);
void bindIconInit(MenuItem_Typedef* node , iconInfo_Typedef *argIcon);

void tree_node_binding_oneTime(u16_t cnt, MenuItem_Typedef *non_leaf,...);
u8_t get_menu_choose_cnt(u8_t id);
u8_t get_uplist_from_curlisthead(curHandle_Typedef *handle);
void currentFace_refresh(curHandle_Typedef *handle);
void enterExit_to_newPage(curHandle_Typedef *handle, u8_t mode);
MenuItem_Typedef* uintCreate(u8_t id , const char *text);
void free_branch_auto(MenuItem_Typedef* non_lef);
void currentHandleInit(MenuItem_Typedef * root, curHandle_Typedef *handle);
void chooseCursorUp(curHandle_Typedef *handle);
void chooseCursorDown(curHandle_Typedef *handle);


#ifdef __cplusplus
}
#endif

#endif