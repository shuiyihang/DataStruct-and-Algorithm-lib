#include "pageManager.h"

MenuItem_Typedef *nodelist[NODE_MAX];

extern curHandle_Typedef menuHandle;



static u8_t now_page_id;
static u8_t new_page_id;

void shiftPage(u8_t id)
{
    new_page_id = id;
}
u8_t getCurPage(void)
{
    return now_page_id;
}










//公用
void NodeRegister(NodeID id, keyCb keyEventProgress, nonParamCb Setup, nonParamCb loop, nonParamCb Exit)
{
    nodelist[id]->keyEventProgress = keyEventProgress;
    nodelist[id]->setup = Setup;
    nodelist[id]->loop = loop;
    nodelist[id]->exit = Exit;
}


void NodeStatusRun()
{
    if(now_page_id != new_page_id)//发生了页面的切换
    {
        //旧页面exit
        nodelist[now_page_id]->exit();
        //新页面setup
        nodelist[new_page_id]->setup();      
    
        now_page_id = new_page_id;
        menuHandle.chosse_cnt = get_menu_choose_cnt(now_page_id);
    }
    // if(menuHandle.need_refresh){//需要刷新时做一次刷新
    //     nodelist[now_page_id]->loop();
    // }
    nodelist[now_page_id]->loop();

    

    //执行主循环
}

void NodeKeyDeal(keybuff_Typedef *buff)
{
    nodelist[now_page_id]->keyEventProgress(buff);
}


void NodeInit()
{
    /*节点堆内存分配,指定处理函数和相关信息*/
    NODE_REG(root);
    NODE_REG(about);
    // NODE_REG(bluetooth);
    // NODE_REG(setParam);
    // NODE_REG(multiSelt);
    // NODE_REG(game);
    now_page_id = root;
    /*节点父子关系绑定*/
    tree_node_binding_oneTime(1,nodelist[root],nodelist[about]);

    /*显示首页*/
}


//电量等事件 在 消息页面有空闲的时候可以进行通知，并且一段时间之后没处理，要自动切回原来显示