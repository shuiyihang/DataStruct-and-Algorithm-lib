
#include "control_mod.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
//把控件放到页面里
void place_module_to_page(page_info *page,module_info *mod)
{
    single_list_add_tail(&mod->node,&page->page_start);
}


void temp_simluate_show_mod(unsigned char x,unsigned char y,const char* str)
{
    for(int i=0;i<y;i++){
        printf("\n");
    }
    for(int j=0;j<x;j++){
        printf(" ");
    }
    printf("%s\n",str);
}
//显示一个页面的所有控件
void place_all_module_on_page(page_info *page,unsigned char mode)//
{
    module_info *mod;
    if(mode){//所有控件重新摆放
        single_list_for_each_entry(mod,&page->page_start,node){
            switch (mod->mod_type)
            {
            case BUTTON_MOD:
                button *b_mod = list_entry(mod,button,info);
                printf("button:<%d,%d>:%s\n",b_mod->x,b_mod->y,b_mod->text);

                break;
            case SCROLL_MOD:
                scrollbar *bar_mod = list_entry(mod,scrollbar,info);
                printf("scrollbar:<%d,%d>\n",bar_mod->x,bar_mod->y);
                break;
            case RING_MOD:

                break;
            case TIME_TUBE_MOD:

                break;
            
            default:
                break;
            }
            
        }
    }else{//只重新布置动态控件
        single_list_for_each_entry(mod,&page->page_start,node){
            if(mod->dny_or_static){
                
            }
        }
    }

}

void key_event_deal()
{
    //点击了确定，当前选择停留在哪个控件上，执行控件的绑定函数
}

//有哪些控件是可以进行选择的:像是按键
//有哪些控件是需要动态刷新的: 像是圆环，时间
//有哪些控件是固定的:像是一些logo啥的

//需要什么控件