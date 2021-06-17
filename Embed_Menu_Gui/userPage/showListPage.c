
#include "../menuGuiLib/pageManager.h"
#include "allPage.h"



static void Setup()
{

}

static void loop()
{
    

    MenuItem_Typedef *temp;
    temp = nodelist[menuHandle.cur_node_id];

    const struct single_list_head *list_node = &temp->localPos;
    
    u8_t cnt = 0;
    u8_t labelNum = 0;
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
    
    printf("================\t\n");
}

static void Exit()
{

}

static void keyEvent(u8_t key)
{
    switch (key)//按键只负责参数的更新,界面更新由单独函数控制
        {
        case 'w'://光标向上
            if(__get_node_type(menuHandle.cur_type) == NON_LEAF_SIGN){
                chooseCursorUp(&menuHandle);
            } 
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
    nodelist[id] = uintCreate(NON_LEAF,"根目录");
    NodeRegister(id , keyEvent, Setup, loop, Exit);
}

//使用同一处理函数的写在下面即可