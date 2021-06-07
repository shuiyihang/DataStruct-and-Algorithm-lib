#ifndef __MENUBASE_H__
#define __MENUBASE_H__

#include "string.h"
#include "stdarg.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "../Common_Struct_lib/Hi_single_list.h"
#include "../Common_Struct_lib/Hi_Queue.h"


#define True    1
#define False   0

#define PAGE_NUMS   4   //一页最多容纳 3 行

typedef struct MenuItem MenuItem_Typedef;

typedef void (*show_dir_page)(const MenuItem_Typedef *menu);

typedef void (*show_leaf_page)( MenuItem_Typedef *leaf);

typedef void (*updataConfig)( MenuItem_Typedef *leaf , int test);




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
    u8_t unitType;          //该菜单节点的信息

    s8_t selectNum;         //选中的条目序号
    s8_t cursorPos;         //光标位置
    const char *briefInfo;  //子菜单标题信息
    const iconInfo_Typedef *icon;       //子菜单的图标信息
    const char *cur_icon;
    struct single_list_head  localPos;  //绑定子目录的头节点
    struct single_list_head  brother;
    struct single_list_head *parentPtr;
    union{
        show_dir_page   showMenu;
        show_leaf_page  endPageDeal;    //包括 显示静态/动态页面和全局配置修改后调用上一次刷新配置页面
        updataConfig    renewConfigDeal;
    };
    union 
    {
        int param;//针对可以编辑参数的节点
    };
    
}MenuItem_Typedef;



#define LEAF_TYPE_BIT   (6)
#define LEAF_MUTLI_BIT  (5)
#define LEAF_STATE_BIT  (4)


#define LEAF_INIT_STATE     (1 << LEAF_STATE_BIT)
#define MULTI_LEAF_ASSERT   (1 << LEAF_MUTLI_BIT)
#define CAN_ENTER_ASSERT    (3 << LEAF_TYPE_BIT)
#define NEED_DYN_ASSERT     (1)


/**
 * 位值代表
 * 0叶子/1非叶子  1展开/0不能展开    1多选/0单选            默认状态1开/0关
 * null,         null,            1可以编辑/0不能编辑     1需要动态刷新/0静态显示的
*/

typedef enum {
    NON_LEAF = 0x80,//非叶子节点
    NON_LEAF_EDIT_EN = 0x82,
    LEAF_OPEN = 0x40 ,//可以展开的叶子节点
    LEAF_OPEN_DYN = 0x41,//需要动态刷新的可以展开的叶子节点
    LEAF_OPEN_EDIT_EN = 0x42,//可以进行参数编辑可以展开的叶子节点
    LEAF_CLOSE_MULTI_DISEN = 0x20,//不能展开的叶子节点, 支持多选, 默认状态是关
    LEAF_CLOSE_MULTI_EN = 0x30,//不能展开的叶子节点, 支持多选, 默认状态是开
    LEAF_CLOSE_NOMULTI_DISEN = 0,//不能展开的叶子节点, 不支持多选, 默认状态是关
    LEAF_CLOSE_NOMULTI_EN   = 0x10,//不能展开的叶子节点, 不支持多选, 默认状态是开
    LEAF_CLOSE_EDIT_EN = 0x03,
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
    u8_t cur_type;
    u8_t chosse_cnt;
    u8_t show_cnt;//显示的目数
    u8_t need_refresh;//是否需要刷屏

    s8_t cur_choose;
    s8_t startItem;//顶叶序号
    s8_t cursorPos;//光标位置
    
    struct single_list_head *cur_list_head;//指向菜单的头节点
}curHandle_Typedef;



static inline u8_t __get_node_type(u8_t multi)
{
    return (multi>>LEAF_TYPE_BIT)&3;
};

void tree_node_binding_oneTime(u16_t cnt, MenuItem_Typedef *non_leaf,...);
u8_t get_menu_choose_cnt(curHandle_Typedef *handle);
u8_t get_uplist_from_curlisthead(curHandle_Typedef *handle);
void currentFace_refresh(curHandle_Typedef *handle);
void select_verify_deal(curHandle_Typedef *handle);
void enterExit_to_newPage(curHandle_Typedef *handle, u8_t mode);
MenuItem_Typedef* branchCreate(NODE_TYPE nodeType , const char *text, show_dir_page cb);
MenuItem_Typedef* leafCreate(NODE_TYPE nodeType, const char *text, void* cb , iconInfo_Typedef *argIcon);
void free_branch_auto(MenuItem_Typedef* non_lef);
void currentHandleInit(MenuItem_Typedef * root, curHandle_Typedef *handle);
void chooseCursorUp(curHandle_Typedef *handle);
void chooseCursorDown(curHandle_Typedef *handle);
void updata_pid_param(curHandle_Typedef *handle, u8_t rise);


#ifdef __cplusplus
}
#endif

#endif