#ifndef __USERPAGE_H__
#define __USERPAGE_H__

#include "time.h"
#include "menuBase.h"

#ifdef __cplusplus
extern "C" {
#endif


extern configSet_Typedef *operat_config;
extern time_t *timep;
extern curHandle_Typedef menuHandle;

void simulate_show_list_page(const MenuItem_Typedef *menu);
void simulate_show_option_icon(const MenuItem_Typedef *menu);
void blueTooth_page_deal( MenuItem_Typedef *leaf);
void aboutPhone_page( MenuItem_Typedef *leaf);
void test_turn_page(MenuItem_Typedef *leaf);
void show_dynamic_time_page(MenuItem_Typedef *leaf);
void autoCorrct_page_deal( MenuItem_Typedef *leaf);
void oneHandle_page_deal( MenuItem_Typedef *leaf);
void glide_page_deal( MenuItem_Typedef *leaf);


#ifdef __cplusplus
}
#endif

#endif