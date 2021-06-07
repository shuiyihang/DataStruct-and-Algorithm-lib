#include "menuBase.h"
#include "stdlib.h"
#include "stdio.h"

#include "userPage.h"

configSet_Typedef *operat_config;
curHandle_Typedef menuHandle;
//定义开关的风格
iconInfo_Typedef text_onoff = {
    .on_icon = "开",
    .off_icon = "关",
};

iconInfo_Typedef sign_onoff = {
    .on_icon = "√",
    .off_icon = " ",
};

void main()
{
    MenuItem_Typedef *rootNode;
    MenuItem_Typedef *UniversalNode;
    MenuItem_Typedef *BluetoothNode;
    MenuItem_Typedef *PhoneNode;
    MenuItem_Typedef *TimeNode;
    MenuItem_Typedef *KeyNode;
    MenuItem_Typedef *CorrectNode;
    MenuItem_Typedef *oneHandleNode;
    MenuItem_Typedef *slideInputNode;

    MenuItem_Typedef *BluetoothNode_1, *CorrectNode_1, *slideInputNode_1, *oneHandleNode_1, *oneHandleNode_2, *oneHandleNode_3;

    MenuItem_Typedef *NotifyNode, *HotsportNode, *NoDisturbNode, *PidNode;

    MenuItem_Typedef *P_param, *I_param ,*D_param;

    u8_t cmd;

    MenuItem_Typedef *deal_special_page;

    

    operat_config = (configSet_Typedef*)malloc(sizeof(configSet_Typedef));
    memset(operat_config, 0, sizeof(configSet_Typedef));
    
    rootNode = branchCreate(NON_LEAF,"设置",simulate_show_list_page);
    UniversalNode = branchCreate(NON_LEAF,"通用",simulate_show_list_page);
    KeyNode = branchCreate(NON_LEAF,"键盘",simulate_show_list_page);

    BluetoothNode = branchCreate(NON_LEAF,"蓝牙",simulate_show_option_icon);//增加蓝牙开关控制节点

    //添加,翻页测试使用
    PidNode = branchCreate(NON_LEAF_EDIT_EN,"调参",simulate_edit_param_task);//静态显示的


    P_param = leafCreate(LEAF_CLOSE_EDIT_EN, "P",test_turn_page,NULL);
    I_param = leafCreate(LEAF_CLOSE_EDIT_EN, "I",test_turn_page,NULL);
    D_param = leafCreate(LEAF_CLOSE_EDIT_EN, "D",test_turn_page,NULL);


    NotifyNode = leafCreate(LEAF_OPEN, "通知",test_turn_page,NULL);//静态显示的
    HotsportNode = leafCreate(LEAF_OPEN, "个人热点",test_turn_page,NULL);//静态显示的
    NoDisturbNode = leafCreate(LEAF_OPEN, "勿扰模式",test_turn_page,NULL);//静态显示的
    /////////////

    PhoneNode = leafCreate(LEAF_OPEN, "关于本机",aboutPhone_page,NULL);//静态显示的
    TimeNode = leafCreate(LEAF_OPEN,"时间",show_dynamic_time_page,NULL);//动态显示

    CorrectNode = branchCreate(NON_LEAF,"自动改正",simulate_show_option_icon);//增加开关控制节点
    oneHandleNode = branchCreate(NON_LEAF,"单手键盘",simulate_show_option_icon);//增加开关控制节点
    slideInputNode = branchCreate(NON_LEAF,"滑行键入",simulate_show_option_icon);



    
    BluetoothNode_1 = leafCreate(LEAF_CLOSE_MULTI_DISEN, "蓝牙",blueTooth_page_deal, &text_onoff);//增加蓝牙开关控制节点 
    CorrectNode_1 = leafCreate(LEAF_CLOSE_MULTI_DISEN, "自动改正",autoCorrct_page_deal, &text_onoff);
    slideInputNode_1 = leafCreate(LEAF_CLOSE_MULTI_DISEN, "滑行键入",glide_page_deal,&text_onoff);
    oneHandleNode_1 = leafCreate(LEAF_CLOSE_NOMULTI_EN, "左",oneHandle_page_deal, &sign_onoff);
    oneHandleNode_2 = leafCreate(LEAF_CLOSE_NOMULTI_DISEN, "中",oneHandle_page_deal, &sign_onoff);
    oneHandleNode_3 = leafCreate(LEAF_CLOSE_NOMULTI_DISEN, "右",oneHandle_page_deal, &sign_onoff);





    tree_node_binding_oneTime(7, rootNode,BluetoothNode,PidNode,UniversalNode,NotifyNode,HotsportNode,NoDisturbNode,TimeNode);

    tree_node_binding_oneTime(2, UniversalNode,PhoneNode,KeyNode);

    tree_node_binding_oneTime(3, KeyNode,CorrectNode,
                                oneHandleNode,slideInputNode);

    tree_node_binding_oneTime(1,BluetoothNode,BluetoothNode_1);
    tree_node_binding_oneTime(1,CorrectNode,CorrectNode_1);
    tree_node_binding_oneTime(1,slideInputNode,slideInputNode_1);
    tree_node_binding_oneTime(3,oneHandleNode,oneHandleNode_1,oneHandleNode_2,oneHandleNode_3);

    tree_node_binding_oneTime(3, PidNode,P_param,I_param,D_param);

    
    currentHandleInit(rootNode,&menuHandle);

    

    while(1)
    {
        if(menuHandle.need_refresh){
            menuHandle.need_refresh = 0;
            currentFace_refresh(&menuHandle);
        }

        cmd = getchar();
        system("stty echo");
        switch (cmd)
        {
        case 'w'://光标向上
            if(__get_node_type(menuHandle.cur_type) == NON_LEAF_SIGN)
            {
                if(operat_config->edit_mode){//处于可编辑模式
                    updata_pid_param(&menuHandle,1);
                }else{
                    chooseCursorUp(&menuHandle);
                    // printf("现在的选择:%d,光标:%d,开始条目:%d\n",menuHandle.cur_choose,menuHandle.cursorPos,menuHandle.startItem);
                    menuHandle.need_refresh = 1;
                }
            }
            break;
        case 'a'://返回
            if(operat_config->edit_mode){
                operat_config->edit_mode = 0;
                menuHandle.need_refresh = 1;
            }else{
                enterExit_to_newPage(&menuHandle,RETURN_PAGE);
            }
            break;
        case 's'://光标向下
            if(__get_node_type(menuHandle.cur_type) == NON_LEAF_SIGN)
            {
                if(operat_config->edit_mode){//处于可编辑模式
                    updata_pid_param(&menuHandle,0);
                }else{
                    chooseCursorDown(&menuHandle);
                    // printf("现在的选择:%d,光标:%d,开始条目:%d\n",menuHandle.cur_choose,menuHandle.cursorPos,menuHandle.startItem);
                    menuHandle.need_refresh = 1;
                }
            }
            break;
        case 'd'://进入 
            enterExit_to_newPage(&menuHandle,ENTER_PAGE);
            break;
        case 'e':
            //1 执行对应deal函数
            //2 刷新当前FALSE_NON_LEAF页面
            if(menuHandle.cur_type != NON_LEAF_EDIT_EN){
                select_verify_deal(&menuHandle);
            }else{
                //进入编辑模式
                operat_config->edit_mode = 1;
                menuHandle.need_refresh = 1;
            }
            
            break;//确认键
        default:
            break;
        }
        if(cmd == 'q'){
            
            break;
        }
        cmd = 'l';

        


    }

    free_branch_auto(rootNode);
    free(operat_config);
}












