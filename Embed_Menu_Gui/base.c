#include "./menuGuiLib/menuBase.h"
#include "stdlib.h"
#include "stdio.h"

#include "./menuGuiLib/config.h"

#include "./menuGuiLib/pageManager.h"

curHandle_Typedef menuHandle;

extern MenuItem_Typedef *nodelist[NODE_MAX];

void main()
{
    u8_t cmd;
    
    NodeInit();
    currentHandleInit(nodelist[root],&menuHandle);
    while(1)
    {
        NodeStatusRun();
        system("stty echo");
        cmd = getchar();
        

        NodeKeyDeal(cmd);
        
        if(cmd == 'q'){
            
            break;
        }
        cmd = 'l';
    }

    free_branch_auto(nodelist[root]);
}












