#include "pageManager.h"

MenuItem_Typedef *nodelist[NODE_MAX];

extern curHandle_Typedef menuHandle;

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
    if(menuHandle.page_switch)//发生了页面的切换
    {
        //旧页面exit
        nodelist[menuHandle.exit_id]->exit();
        //新页面setup
        nodelist[menuHandle.cur_node_id]->setup();      
    
        menuHandle.page_switch = 0;//完成切页

        menuHandle.need_refresh = 1;//仅作测试
    }
    if(menuHandle.need_refresh){//需要刷新时做一次刷新
        nodelist[menuHandle.cur_node_id]->loop();
    }

    if(menuHandle.page_anim){//需要做动画处理
        

    }//或者立即显示出来
    

    //执行主循环
}

void NodeKeyDeal(keybuff_Typedef *buff)
{
    nodelist[menuHandle.cur_node_id]->keyEventProgress(buff);
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
    /*节点父子关系绑定*/
    tree_node_binding_oneTime(1,nodelist[root],nodelist[about]);

    /*显示首页*/
}


//电量等事件 在 消息页面有空闲的时候可以进行通知，并且一段时间之后没处理，要自动切回原来显示