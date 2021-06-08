
#include "menuBase.h"

//建树
//通过指定父亲和儿子来构件关系
//通过不定参数一次性绑定
void tree_node_binding_oneTime(u16_t cnt, MenuItem_Typedef *non_leaf,...)
{
    va_list argPtr;
    MenuItem_Typedef *temp;
    va_start(argPtr,non_leaf);
    while(cnt--)
    {
        temp = va_arg(argPtr,MenuItem_Typedef *);
        single_list_add_tail(&temp->brother,&non_leaf->localPos);
        temp->parentPtr = &non_leaf->localPos;
    }
    va_end(argPtr);
}


u8_t get_menu_choose_cnt(curHandle_Typedef *handle)
{
    u8_t cnt=0;
    struct single_list_head* temp = handle->cur_list_head->next;
    while(temp){
        cnt++;
        temp = temp->next;
    }
    return cnt;
}

u8_t get_uplist_from_curlisthead(curHandle_Typedef *handle)
{
    MenuItem_Typedef *pos;
    struct single_list_head *ptr = handle->cur_list_head;
    pos = list_entry(ptr,MenuItem_Typedef,localPos);

    if(pos->parentPtr == NULL){
        return False;
    }
    handle->cur_list_head = pos->parentPtr;
    handle->chosse_cnt = get_menu_choose_cnt(handle);//注意摆放的位置

    pos = list_entry(handle->cur_list_head,MenuItem_Typedef,localPos);
    handle->cur_choose = pos->selectNum;
    handle->cursorPos = pos->cursorPos;
    handle->startItem = pos->selectNum - pos->cursorPos;

    // printf("返回上一层现在的选择:%d,光标:%d,开始条目:%d\n",handle->cur_choose,handle->cursorPos,handle->startItem);
    return True;
}



void currentFace_refresh(curHandle_Typedef *handle)
{
    MenuItem_Typedef *pos;
    struct single_list_head *ptr = handle->cur_list_head;

    system("clear");
    pos = list_entry(ptr,MenuItem_Typedef,localPos);

    handle->cur_type = pos->unitType;
    if(__get_node_type(pos->unitType) == OPEN_LEAF_SIGN){
        ((show_leaf_page)pos->cb)(pos);
    }else if(__get_node_type(pos->unitType) == NON_LEAF_SIGN){
        ((show_dir_page)pos->cb)(pos);
    }
}


void select_verify_deal(curHandle_Typedef *handle)
{
    MenuItem_Typedef *pos = NULL;
    u8_t cnt = 0;
    struct single_list_head *ptr = handle->cur_list_head;

    single_list_for_each_entry(pos,ptr,brother)
    {
        if(__get_node_type(pos->unitType) != CLOSE_LEAF_SIGN){
                return;//
        }
        if(cnt == handle->cur_choose){
            
            ((updataConfig)pos->cb)(pos,10);//改变配置参数

            if(pos->unitType&LEAF_MULTI_ASSERT)//如果支持多选,可以直接跳出去了
                break;
        }else{
            if(!(pos->unitType&LEAF_MULTI_ASSERT)){//不支持多选
                printf("%s\n",pos->briefInfo);
                pos->cur_icon = pos->icon->off_icon;
            }
        }
        cnt++;
    }
    handle->need_refresh = 1;

}


void updata_Binding_param(curHandle_Typedef *handle, u8_t rise)
{
    MenuItem_Typedef *pos;
    u8_t cnt = 0;
    struct single_list_head *ptr = handle->cur_list_head;
    printf("pid:%d\n",handle->cur_choose);
    single_list_for_each_entry(pos,ptr,brother)
    {
        // if(__get_node_type(pos->unitType) != CLOSE_LEAF_SIGN){
        //         return;//
        // }
        if(cnt == handle->cur_choose){
            if(rise){
                (*(int *)pos->param)++;//参数应该转成什么类型用户是清楚的
            }else{
                if((*(int *)pos->param) > 0){
                    (*(int *)pos->param)--;
                }
            }
            break;
        }
        cnt++;
    }

    handle->need_refresh = 1;
}


void enterExit_to_newPage(curHandle_Typedef *handle, u8_t mode)
{
    MenuItem_Typedef *pos,*save;
    u8_t cnt = 0;
    struct single_list_head *ptr = handle->cur_list_head;
    if(mode == ENTER_PAGE){

        save = list_entry(ptr,MenuItem_Typedef,localPos);
        printf("%d\n",save->unitType);
        if(__get_node_type(save->unitType) == CLOSE_LEAF_SIGN){
            printf("false non leaf\n");
            return;
        }
        if(ptr->next){//非空进入下一个页面
            single_list_for_each_entry(pos,ptr,brother)
            {
                if(cnt == handle->cur_choose && __get_node_type(pos->unitType) != CLOSE_LEAF_SIGN){

                    save->cursorPos = handle->cursorPos;//提前保存一下
                    save->selectNum = handle->cur_choose;

                    handle->cur_choose = 0;//
                    handle->cursorPos = 0;
                    handle->startItem = 0;
                    handle->cur_list_head = &pos->localPos;//重新初始list
                    
                    handle->chosse_cnt = get_menu_choose_cnt(handle);//注意摆放的位置
                    break;
                }
                cnt++;
            }
            handle->need_refresh = 1;
        }
    }else{//返回上一级
        if(get_uplist_from_curlisthead(handle))
            handle->need_refresh = 1;
    }

    
}


MenuItem_Typedef* uintCreate(NODE_TYPE nodeType , const char *text, void* cb)
{
    MenuItem_Typedef* non_leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(non_leaf == NULL){
        return NULL;
    }
    memset(non_leaf,0,sizeof(MenuItem_Typedef));
    non_leaf->briefInfo = text;
    non_leaf->unitType = nodeType;
    non_leaf->cb = cb;
    
    return non_leaf;
} 

void bindParamInit(MenuItem_Typedef* node, void *bindParam)
{
    node->param = bindParam;
}

void bindIconInit(MenuItem_Typedef* node , iconInfo_Typedef *argIcon)
{
    node->icon = argIcon;
    if(argIcon){
        if(node->unitType&LEAF_INIT_STATE){
            node->cur_icon = argIcon->on_icon;
        }else{
            node->cur_icon = argIcon->off_icon;
        }
    }
}

//应该只初始化共有属性，特殊属性另写函数单独初始化

void free_branch_auto(MenuItem_Typedef* non_lef)
{
    struct single_list_head *ptr = &non_lef->localPos;
    MenuItem_Typedef *temp;
    
    single_list_for_each_entry(temp,ptr,brother){
        if(__get_node_type(temp->unitType) == OPEN_LEAF_SIGN){
            free(temp);
        }else{
            free_branch_auto(temp);
        }
    }
    free(non_lef);
}


void currentHandleInit(MenuItem_Typedef * root, curHandle_Typedef *handle)
{
    memset(handle,0,sizeof(curHandle_Typedef));
    handle->cur_list_head = &root->localPos;
    handle->cur_type = root->unitType;
    handle->chosse_cnt = get_menu_choose_cnt(handle);
    handle->need_refresh = 1;
}


void chooseCursorUp(curHandle_Typedef *handle)
{
    if(handle->cur_choose>0){
        handle->cur_choose--;
    }

    handle->cursorPos--;
    if(handle->chosse_cnt <= PAGE_NUMS){
        handle->show_cnt = handle->chosse_cnt;
        if(handle->cursorPos < 0){
            handle->cursorPos = 0;//diff
        }
    }else{
        handle->show_cnt = PAGE_NUMS;
        if(handle->cursorPos < 0){
            handle->cursorPos = 0;
            if(handle->startItem > 0 ){
                handle->startItem--;
            }
        }
    }
    handle->need_refresh = 1;
}


void chooseCursorDown(curHandle_Typedef *handle)
{
    if(handle->cur_choose < handle->chosse_cnt-1){
        handle->cur_choose++;
    }

    handle->cursorPos++;
    if(handle->chosse_cnt <= PAGE_NUMS){
        handle->show_cnt = handle->chosse_cnt;
        if(handle->cursorPos >= handle->show_cnt){
            handle->cursorPos = handle->show_cnt-1;//diff
        }
    }else{
        handle->show_cnt = PAGE_NUMS;
        if(handle->cursorPos >= PAGE_NUMS){
            handle->cursorPos = PAGE_NUMS-1;
            if(handle->startItem < handle->chosse_cnt - PAGE_NUMS){
                handle->startItem++;
            }
        }
    }
    handle->need_refresh = 1;
}


void key_dispatch_cb_deal(curHandle_Typedef *handle, u8_t key)
{
    MenuItem_Typedef *pos;
    struct single_list_head *ptr = handle->cur_list_head;
    pos = list_entry(ptr,MenuItem_Typedef,localPos);
    ((game_deal_page)pos->cb)(pos,key);
}
