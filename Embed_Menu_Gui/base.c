#include "./menuGuiLib/menuBase.h"
#include "stdlib.h"
#include "stdio.h"

#include "./menuGuiLib/config.h"

#include "./menuGuiLib/pageManager.h"

curHandle_Typedef menuHandle;

extern MenuItem_Typedef *nodelist[NODE_MAX];

keybuff_Typedef buff;

void main()
{
    u8_t cmd;
    
    NodeInit();//完成节点的申请和绑定
    currentHandleInit(nodelist[root],&menuHandle);

    keybuffInit(&buff);
    while(1)
    {
        NodeStatusRun();
        system("stty echo");
        cmd = getchar();
        
        putKeyToBuff(&buff,cmd);

        NodeKeyDeal(&buff);
        
        if(cmd == 'q'){
            
            break;
        }
        cmd = 'l';
    }

    free_branch_auto(nodelist[root]);
}












