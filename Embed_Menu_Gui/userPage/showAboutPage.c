
#include "../menuGuiLib/pageManager.h"
#include "allPage.h"



static void Setup()
{

}

static void loop()
{
    MenuItem_Typedef *temp;
    u8_t cur_page_id = getCurPage();
    temp = nodelist[cur_page_id];
    system("clear");
    printf("======%s======\n",temp->briefInfo);
    printf("[名称:      蝴蝶与猫]\n");
    printf("[软件版本:  14.5.1]\n");
    printf("[运营商:    中国电信]\n");
    printf("====================\n");
    menuHandle.need_refresh = 0;
}

static void Exit()
{

}

static void keyEventProgress(keybuff_Typedef *buff)
{
    if(keybuffIsEmpty(buff)){
        return;
    }
    u8_t key = getKeyFromBuff(buff);
    switch (key)//按键只负责参数的更新,界面更新由单独函数控制
        {
        case 'w'://光标向上
            //文字向上
            break;
        case 'a'://返回
            enterExit_to_newPage(&menuHandle,RETURN_PAGE);
            break;
        case 's'://光标向下
            //文字向下
            break;
        default:
            break;
        }

}



//注册使用函数
void NodeRegister_page_about(NodeID id)
{
    nodelist[id] = uintCreate(id,"关于本机");
    NodeRegister(id , keyEventProgress, Setup, loop, Exit);
}

//使用同一处理函数的写在下面即可