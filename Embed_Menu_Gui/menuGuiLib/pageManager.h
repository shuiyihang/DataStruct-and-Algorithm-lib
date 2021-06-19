//不同页面使用的公共部分


#ifndef __PAGEMANAGER_H__
#define __PAGEMANAGER_H__

#include "config.h"
#include "menuBase.h"

#ifdef __cplusplus
extern "C" {
#endif

extern MenuItem_Typedef *nodelist[NODE_MAX];



#define NODE_REG(name) \
do{ \
    NodeRegister_page_##name(name);\
}while(0)


void NodeRegister(NodeID id, keyCb keyEventProgress, nonParamCb Setup, nonParamCb loop, nonParamCb Exit);

void NodeStatusRun();
void NodeKeyDeal(keybuff_Typedef *buff);
void NodeInit();

void shiftPage(u8_t id);
u8_t getCurPage(void);


#ifdef __cplusplus
}
#endif

#endif