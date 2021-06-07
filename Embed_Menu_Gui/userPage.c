
#include "userPage.h"


/**
 * 
 * 以下仅为界面的测试,与核心无关
*/

//一种就是这种简单的只显示菜单项的方式
void simulate_show_list_page(const MenuItem_Typedef *menu)//由非叶子节点调用，显示子页面内容
{
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    u8_t cnt = 0;
    u8_t labelNum = 0;
    printf("======%s======\t\n",menu->briefInfo);
    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt >= menuHandle.startItem){
            if(labelNum == menuHandle.cursorPos){
                printf("==>:%s\t\n",temp->briefInfo);
            }else{
                printf("    %s\t\n",temp->briefInfo);
            }
            labelNum++;
            if(labelNum >= PAGE_NUMS){
                break;
            }
        }
        cnt++;
    }

    while (labelNum < PAGE_NUMS)
    {
        labelNum++;
        printf("\n");
    }
    
    printf("================\t\n");
}



void simulate_show_option_icon(const MenuItem_Typedef *menu )
{
    //这个页面由具有选择项子页面的非叶子节点调用
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    u8_t cnt = 0;
    printf("======%s======\t\n",menu->briefInfo);
    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt == menuHandle.cur_choose)
            printf("==>:%s           %s\t\n",temp->briefInfo,temp->cur_icon);//有问题
        else
            printf("    %s           %s\t\n",temp->briefInfo,temp->cur_icon);
        
        cnt++;
    }
    while (cnt < PAGE_NUMS)
    {
        cnt++;
        printf("\n");
    }
    printf("================\t\n");
}

void simulate_edit_param_task(const MenuItem_Typedef *menu)
{
    //这个页里面进行参数编辑处理
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    u8_t cnt = 0;
    if(operat_config->edit_mode){
        printf("==%s(%s)==\t\n",menu->briefInfo,"编辑模式");
    }else{
        printf("======%s======\t\n",menu->briefInfo);
    }
    

    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt == menuHandle.cur_choose)
            printf("==>:%s           %d\t\n",temp->briefInfo,temp->param);//有问题
        else
            printf("    %s           %d\t\n",temp->briefInfo,temp->param);
        
        cnt++;
    }
    while (cnt < PAGE_NUMS)
    {
        cnt++;
        printf("\n");
    }
    printf("================\t\n");
}



void blueTooth_page_deal( MenuItem_Typedef *leaf)
{
    operat_config->bt_state = ~operat_config->bt_state;
    if(operat_config->bt_state){
        leaf->cur_icon = leaf->icon->on_icon;
    }else{
        leaf->cur_icon = leaf->icon->off_icon;
    }
}


void aboutPhone_page( MenuItem_Typedef *leaf)//如果是一段长文本的话,应该也支持翻页
{
    printf("======%s======\n",leaf->briefInfo);
    printf("[名称:      蝴蝶与猫]\n");
    printf("[软件版本:  14.5.1]\n");
    printf("[运营商:    中国电信]\n");
    printf("====================\n");
}

void test_turn_page(MenuItem_Typedef *leaf)
{
    printf("======%s======\n",leaf->briefInfo);
    printf("[翻页测试:     .... ]\n");
    printf("====================\n");
}

void show_dynamic_time_page(MenuItem_Typedef *leaf, int test)
{
    printf("======%s======\n",leaf->briefInfo);
    printf("[动态显示测试:     .... ]\n");
    printf("====================\n");
    
}


void autoCorrct_page_deal( MenuItem_Typedef *leaf)
{
    operat_config->correct_state = ~operat_config->correct_state;
    if(operat_config->correct_state){
        leaf->cur_icon = leaf->icon->on_icon;
    }else{
        leaf->cur_icon = leaf->icon->off_icon;
    }
    
}


void oneHandle_page_deal( MenuItem_Typedef *leaf)//这种情况是3选一
{
    operat_config->oneHandle_state = (++operat_config->oneHandle_state)%3;
    //只能单选

    leaf->cur_icon = leaf->icon->on_icon; 
}


void glide_page_deal( MenuItem_Typedef *leaf)
{
    operat_config->glid_state = ~operat_config->glid_state;
    if(operat_config->glid_state){
        leaf->cur_icon = leaf->icon->on_icon;
    }else{
        leaf->cur_icon = leaf->icon->off_icon;
    }
}