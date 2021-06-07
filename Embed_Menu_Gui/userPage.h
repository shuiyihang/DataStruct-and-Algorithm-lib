#ifndef __USERPAGE_H__
#define __USERPAGE_H__

#include "time.h"
#include "menuBase.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct configSet
{
    u8_t bt_state;//蓝牙开关状态
    u8_t correct_state;//自动改正开关
    u8_t oneHandle_state;//单手状态
    u8_t glid_state;//是否允许滑动

    u8_t edit_mode;//页面是否处于可编辑模式

}configSet_Typedef;

extern configSet_Typedef *operat_config;
extern curHandle_Typedef menuHandle;

void simulate_show_list_page(const MenuItem_Typedef *menu);
void simulate_show_option_icon(const MenuItem_Typedef *menu);

void simulate_edit_param_task(const MenuItem_Typedef *menu);

void blueTooth_page_deal( MenuItem_Typedef *leaf);
void aboutPhone_page( MenuItem_Typedef *leaf);
void test_turn_page(MenuItem_Typedef *leaf);
void show_dynamic_time_page(MenuItem_Typedef *leaf, int test);
void autoCorrct_page_deal( MenuItem_Typedef *leaf);
void oneHandle_page_deal( MenuItem_Typedef *leaf);
void glide_page_deal( MenuItem_Typedef *leaf);


#ifdef __cplusplus
}
#endif

#endif