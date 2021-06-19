//每个特殊节点的页面处理函数单独来写
//还有可以通用的处理函数

#ifndef __ALLPAGE_H__
#define __ALLPAGE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "../menuGuiLib/menuBase.h"
#include "../menuGuiLib/config.h"

extern curHandle_Typedef menuHandle;

void NodeRegister_page_root(NodeID id);
void NodeRegister_page_about(NodeID id);

#ifdef __cplusplus
}
#endif

#endif