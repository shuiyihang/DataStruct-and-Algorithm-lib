
#include "../menuGuiLib/pageManager.h"
#include "allPage.h"



static void Setup()
{
    //终端页面用来设置传感器参数啥的
}

static void loop()
{
    

    MenuItem_Typedef *temp;
    u8_t cur_page_id = getCurPage();
    temp = nodelist[cur_page_id];

    const struct single_list_head *list_node = &temp->localPos;
    
    u8_t cnt = 0;
    u8_t labelNum = 0;
    system("clear");
    printf("======%s======\t\n",temp->briefInfo);
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
    menuHandle.need_refresh = 0;
    
    printf("================\t\n");
}

static void Exit()
{

}

static void keyEventProgress(keybuff_Typedef *buff)
{
    if(keybuffIsEmpty(buff)){
        printf("key null\n");
        return;
    }
    u8_t key = getKeyFromBuff(buff);
    printf("key :%c\n",key);
    switch (key)//按键只负责参数的更新,界面更新由单独函数控制
        {
        case 'w'://光标向上
            chooseCursorUp(&menuHandle);
            break;
        case 'a'://返回
            enterExit_to_newPage(&menuHandle,RETURN_PAGE);
            break;
        case 's'://光标向下
            chooseCursorDown(&menuHandle);
            break;
        case 'd'://进入 
            enterExit_to_newPage(&menuHandle,ENTER_PAGE);
            break;
        default:
            break;
        }

}



//注册使用函数
void NodeRegister_page_root(NodeID id)
{
    nodelist[id] = uintCreate(id,"根目录");
    NodeRegister(id , keyEventProgress, Setup, loop, Exit);//处理函数
}

//使用同一处理函数的写在下面即可