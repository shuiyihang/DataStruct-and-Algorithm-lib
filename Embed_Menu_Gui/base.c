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

keybuff_Typedef buff;

void main()
{
    MenuItem_Typedef *rootNode;
    MenuItem_Typedef *UniversalNode;
    MenuItem_Typedef *BluetoothNode;
    MenuItem_Typedef *PhoneNode;
    MenuItem_Typedef *KeyNode;
    MenuItem_Typedef *oneHandleNode;

    MenuItem_Typedef *BluetoothNode_1, *oneHandleNode_1, *oneHandleNode_2, *oneHandleNode_3;

    MenuItem_Typedef *GameNode, *PidNode;

    MenuItem_Typedef *P_param, *I_param ,*D_param;

    u8_t cmd;

    MenuItem_Typedef *deal_special_page , *littleDinosaur;

    

    operat_config = (configSet_Typedef*)malloc(sizeof(configSet_Typedef));
    memset(operat_config, 0, sizeof(configSet_Typedef));

    //对全局配置进行一次初始化
    configSetInit(operat_config);

    
    rootNode = uintCreate(NON_LEAF,"设置",simulate_show_list_page);
    UniversalNode = uintCreate(NON_LEAF,"通用",simulate_show_list_page);
    KeyNode = uintCreate(NON_LEAF,"键盘",simulate_show_list_page);

    GameNode = uintCreate(NON_LEAF, "游戏",simulate_show_list_page);//测试可以玩游戏的节点

    BluetoothNode = uintCreate(NON_LEAF_EDIT,"蓝牙",simulate_show_option_icon);//增加蓝牙开关控制节点

    //添加,翻页测试使用
    PidNode = uintCreate(NON_LEAF_EDIT,"调参",simulate_edit_param_task);//这个函数改变的是孩子节点的属性


    P_param = uintCreate(LEAF_CLOSE_EDIT, "P",NULL);
    I_param = uintCreate(LEAF_CLOSE_EDIT, "I",NULL);
    D_param = uintCreate(LEAF_CLOSE_EDIT, "D",NULL);

    //绑定参数
    bindParamInit(P_param,&operat_config->p_pid);
    bindParamInit(I_param,&operat_config->i_pid);
    bindParamInit(D_param,&operat_config->d_pid);

    
    /////////////

    PhoneNode = uintCreate(LEAF_OPEN_STATIC, "关于本机",aboutPhone_page);//静态显示的

    oneHandleNode = uintCreate(NON_LEAF_MULTI_EDIT,"单手键盘",simulate_multi_option_page);//增加开关控制节点
    bindParamInit(oneHandleNode,&operat_config->oneHandle_state);

    littleDinosaur = uintCreate(LEAF_OPEN_EDIT_REFRESH, "小恐龙",game_page_deal);
    
    BluetoothNode_1 = uintCreate(LEAF_CLOSE_OFF, "蓝牙",NULL);//增加蓝牙开关控制节点 
    bindParamInit(BluetoothNode_1,&operat_config->bt_state);
    bindIconInit(BluetoothNode_1 , &text_onoff);
    oneHandleNode_1 = uintCreate(LEAF_CLOSE_ON, "左",NULL);//默认选中左
    bindIconInit(oneHandleNode_1 , &sign_onoff);
    oneHandleNode_2 = uintCreate(LEAF_CLOSE_OFF, "中",NULL);
    bindIconInit(oneHandleNode_2 , &sign_onoff);
    oneHandleNode_3 = uintCreate(LEAF_CLOSE_OFF, "右",NULL);
    bindIconInit(oneHandleNode_3 , &sign_onoff);



    tree_node_binding_oneTime(1,GameNode,littleDinosaur);

    tree_node_binding_oneTime(4, rootNode,GameNode,BluetoothNode,PidNode,UniversalNode);

    tree_node_binding_oneTime(2, UniversalNode,PhoneNode,KeyNode);

    tree_node_binding_oneTime(1, KeyNode,
                                oneHandleNode);

    tree_node_binding_oneTime(1,BluetoothNode,BluetoothNode_1);
    tree_node_binding_oneTime(3,oneHandleNode,oneHandleNode_1,oneHandleNode_2,oneHandleNode_3);

    tree_node_binding_oneTime(3, PidNode,P_param,I_param,D_param);

    
    currentHandleInit(rootNode,&menuHandle);

    keybuffInit(&buff);


    while(1)
    {
        if(menuHandle.need_refresh){
            menuHandle.need_refresh = 0;
            currentFace_refresh(&menuHandle);
        }

        cmd = getchar();
        system("stty echo");
        switch (cmd)//按键只负责参数的更新,界面更新由单独函数控制
        {
        case 'w'://光标向上
            //向缓冲区放值
            if(menuHandle.edit_mode){
                //在展开的叶子节点里面实时把值传给游戏函数
                printf("put key into buff\n");
                putKeyToBuff(&buff,'w');
                menuHandle.need_refresh = 1;
            }else if(__get_node_type(menuHandle.cur_type) == NON_LEAF_SIGN){
                chooseCursorUp(&menuHandle);
            }
            break;
        case 'a'://返回
            if(menuHandle.edit_mode){
                putKeyToBuff(&buff,'a');
                menuHandle.need_refresh = 1;
            }else{
                enterExit_to_newPage(&menuHandle,RETURN_PAGE);
            }
            break;
        case 's'://光标向下
            if(menuHandle.edit_mode){
                //在展开的叶子节点里面实时把值传给游戏函数
                putKeyToBuff(&buff,'s');
                menuHandle.need_refresh = 1;
            }else if(__get_node_type(menuHandle.cur_type) == NON_LEAF_SIGN){
                chooseCursorDown(&menuHandle);
            }
            break;
        case 'd'://进入 
            if(menuHandle.edit_mode){
                putKeyToBuff(&buff,'d');
                menuHandle.need_refresh = 1;
            }else{
                enterExit_to_newPage(&menuHandle,ENTER_PAGE);
            }
            break;
        case 'e':
            if(__node_edit_assert(menuHandle.cur_type)){//有编辑属性
                if(menuHandle.edit_mode){
                    menuHandle.edit_mode = 0;//游戏这里有问题
                    keybuffInit(&buff);
                }else{
                    menuHandle.edit_mode = 1;
                }
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












