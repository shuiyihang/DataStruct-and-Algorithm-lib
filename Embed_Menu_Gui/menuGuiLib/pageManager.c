#include "pageManager.h"

MenuItem_Typedef *nodelist[NODE_MAX];

extern curHandle_Typedef menuHandle;

//公用
void NodeRegister(NodeID id, keyCb keyEvent, nonParamCb Setup, nonParamCb loop, nonParamCb Exit)
{
    nodelist[id]->keyEvent = keyEvent;
    nodelist[id]->setup = Setup;
    nodelist[id]->loop = loop;
    nodelist[id]->exit = Exit;
}


void NodeStatusRun()
{
    // if()//发生了页面的切换
    // {

    // }
    nodelist[menuHandle.cur_node_id]->loop();

    //执行主循环
}

void NodeKeyDeal(u8_t key)
{
    nodelist[menuHandle.cur_node_id]->keyEvent(key);
}


void NodeInit()
{
    /*节点堆内存分配,指定处理函数和相关信息*/
    NODE_REG(root);
    // NODE_REG(about);
    // NODE_REG(bluetooth);
    // NODE_REG(setParam);
    // NODE_REG(multiSelt);
    // NODE_REG(game);
    /*节点父子关系绑定*/
    // tree_node_binding_oneTime(1,nodelist[root],NULL);

    /*显示首页*/
}