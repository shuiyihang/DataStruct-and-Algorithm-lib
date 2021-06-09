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
    u8_t oneHandle_state;//单手状态

    /*测试调参*/
    int p_pid;
    int i_pid;
    int d_pid;


}configSet_Typedef;

extern configSet_Typedef *operat_config;
extern curHandle_Typedef menuHandle;
extern keybuff_Typedef buff;


void configSetInit(configSet_Typedef* cfg);

void simulate_show_list_page(const MenuItem_Typedef *menu);
void simulate_show_option_icon(const MenuItem_Typedef *menu);
void simulate_multi_option_page(const MenuItem_Typedef *menu);

void simulate_edit_param_task(const MenuItem_Typedef *menu);

void blueTooth_page_deal( MenuItem_Typedef *leaf);
void aboutPhone_page( MenuItem_Typedef *leaf);
void oneHandle_page_deal( MenuItem_Typedef *leaf);

void game_page_deal(MenuItem_Typedef *leaf);


#ifdef __cplusplus
}
#endif

#endif