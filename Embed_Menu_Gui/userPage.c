
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
    u8_t keycode;
    if(menuHandle.edit_mode){
        printf("==%s(%s)==\t\n",menu->briefInfo,"编辑模式");
    }else{
        printf("======%s======\t\n",menu->briefInfo);
    }
    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt == menuHandle.cur_choose){
            while(!keybuffIsEmpty(&buff)){
                // printf("key show:%c\n",getKeyFromBuff(&buff));
                keycode = getKeyFromBuff(&buff);
                if(keycode == KEY_UP || keycode == KEY_DOWN){
                    
                    (*(u8_t *)temp->param) = ~(*(u8_t *)temp->param);//注意转换类型
                }
            }
            // printf("操作蓝牙开关:%0x\n",(*(u8_t *)temp->param));
            if((*(u8_t *)temp->param)){
                temp->cur_icon = temp->icon->on_icon;
            }else{
                temp->cur_icon = temp->icon->off_icon;
            }
            printf("==>:%s           %s\t\n",temp->briefInfo,temp->cur_icon);
        }
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


void simulate_multi_option_page(const MenuItem_Typedef *menu)//多选项不支持多选
{
    //这个页面由具有选择项子页面的非叶子节点调用
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    u8_t cnt = 0;
    u8_t keycode;
    u8_t mutli_enable = 0;//默认不支持多选

    u8_t tempSelect = (*(u8_t *)menu->param);
    
    if(__node_multi_assert(menu->unitType)){
        mutli_enable = 1;//支持多选
    }

    if(menuHandle.edit_mode){
        printf("==%s(%s)==\t\n",menu->briefInfo,"编辑模式");
    }else{
        printf("======%s======\t\n",menu->briefInfo);
    }
    single_list_for_each_entry(temp,list_node,brother)
    {
        // printf("=====debug1=====\t\n");
        if(cnt == menuHandle.cur_choose){
            while(!keybuffIsEmpty(&buff)){//只有编辑模式才会进来
                keycode = getKeyFromBuff(&buff);
                if(keycode == KEY_ENTER){
                    if((*(u8_t *)menu->param)&(1 << cnt)){
                        (*(u8_t *)menu->param) &= ((u8_t)~(1 << cnt));
                    }else{     
                        (*(u8_t *)menu->param) |= (1 << cnt);
                    }
                }
            }
            // printf("=====debug2=====\t\n");
            
            if(mutli_enable){//支持多选可以跳出去了
                break;
            }
        }else{
            if(!mutli_enable){//单选的话清除未选项
                (*(u8_t *)menu->param) &= ((u8_t)~(1 << cnt));
            }
            
        }
            
        
        cnt++;
    }

    if(!mutli_enable){//单选模式确保至少有一个选中
        if((*(u8_t *)menu->param) == 0){
            (*(u8_t *)menu->param) = tempSelect;
        }
    }
    
    //更新符号
    cnt = 0;
    single_list_for_each_entry(temp,list_node,brother)
    {
        if((*(u8_t *)menu->param) & (1 << cnt))
        {
            temp->cur_icon = temp->icon->on_icon;
        }else{
            temp->cur_icon = temp->icon->off_icon;
        }
        if(cnt == menuHandle.cur_choose){
            printf("==>:%s           %s\t\n",temp->briefInfo,temp->cur_icon);
        }else{
            printf("    %s           %s\t\n",temp->briefInfo,temp->cur_icon);
        }
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
    u8_t keycode;
    if(menuHandle.edit_mode){
        printf("==%s(%s)==\t\n",menu->briefInfo,"编辑模式");
    }else{
        printf("======%s======\t\n",menu->briefInfo);
    }
    

    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt == menuHandle.cur_choose){
            //处理按键消息
            while(!keybuffIsEmpty(&buff)){
                // printf("key show:%c\n",getKeyFromBuff(&buff));
                keycode = getKeyFromBuff(&buff);
                if(keycode == KEY_UP){
                    (*(int *)temp->param)++;
                }else if(keycode == KEY_DOWN){
                    if((*(int *)temp->param) > 0){
                        (*(int *)temp->param)--;
                    }
                }
            }
            printf("==>:%s           %d\t\n",temp->briefInfo,(*(int *)temp->param));//有问题
        }else{
            printf("    %s           %d\t\n",temp->briefInfo,(*(int *)temp->param));
        }
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



void oneHandle_page_deal( MenuItem_Typedef *leaf)//这种情况是3选一
{
    operat_config->oneHandle_state = (++operat_config->oneHandle_state)%3;
    //只能单选

    leaf->cur_icon = leaf->icon->on_icon; 
}









void configSetInit(configSet_Typedef* cfg)
{
    cfg->p_pid = 15;
    cfg->i_pid = 10;
    cfg->d_pid = 30;
    cfg->oneHandle_state = 1;
}


void game_page_deal(MenuItem_Typedef *leaf)
{
    u8_t keycode;
    if(!menuHandle.edit_mode){
        //初始化游戏界面,等待确认开始
        printf("============\n");
        printf("==等待开始==\n");
        printf("============\n");
        //游戏数据初始化
    }else{
        //正常游戏逻辑处理
        printf("游戏开始\n");
        while(!keybuffIsEmpty(&buff)){
            keycode = getKeyFromBuff(&buff);
            if(keycode == KEY_UP){
                printf("小恐龙跳一跳\n");
            }
        }
    }
}