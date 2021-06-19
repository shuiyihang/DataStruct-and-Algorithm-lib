
#include "menuBase.h"
#include "config.h"
#include "pageManager.h"

extern MenuItem_Typedef *nodelist[NODE_MAX];

void keybuffInit(keybuff_Typedef *buff)
{
    memset(buff,0,sizeof(keybuff_Typedef));
}

u8_t keybuffIsEmpty(keybuff_Typedef *buff)
{
    if(buff->read == buff->write){
        return True;
    }else{
        return False;
    }
}
u8_t keybuffIsFull(keybuff_Typedef *buff)
{
    if((buff->write+1)%BUFF_NUMS == buff->read){
        return True;
    }else{
        return False;
    }
}

u8_t getKeyFromBuff(keybuff_Typedef *buff)
{
    u8_t tempKey = buff->keycode[buff->read];
    if(!keybuffIsEmpty(buff)){
        buff->read = (buff->read+1)%BUFF_NUMS;
        return tempKey;
    }
    return KEY_RESERVED;   
}

void putKeyToBuff(keybuff_Typedef *buff , u8_t key)
{
    if(!keybuffIsFull(buff)){
        buff->keycode[buff->write] = key;
        buff->write = (buff->write+1)%BUFF_NUMS;
    }
}




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


u8_t get_menu_choose_cnt(u8_t id)
{
    u8_t cnt=0;
    struct single_list_head* temp = nodelist[id]->localPos.next;
    while(temp){
        cnt++;
        temp = temp->next;
    }
    return cnt;
}

u8_t get_uplist_from_curlisthead(curHandle_Typedef *handle)
{
    MenuItem_Typedef *pos;
    u8_t cur_page_id = getCurPage();

    pos = nodelist[cur_page_id];

    if(pos->parentPtr == NULL){
        return False;
    }
    pos = list_entry(pos->parentPtr,MenuItem_Typedef,localPos);
    
    handle->cur_choose = pos->selectNum;
    handle->cursorPos = pos->cursorPos;
    handle->startItem = pos->selectNum - pos->cursorPos;

    shiftPage(pos->id);

    // printf("返回上一层现在的选择:%d,光标:%d,开始条目:%d\n",handle->cur_choose,handle->cursorPos,handle->startItem);
    return True;
}



void currentFace_refresh(curHandle_Typedef *handle)
{
    // MenuItem_Typedef *pos;
    // struct single_list_head *ptr = handle->cur_list_head;

    // system("clear");
    // pos = list_entry(ptr,MenuItem_Typedef,localPos);

    // handle->cur_type = pos->unitType;

    //先确定处理函数再确定键值解析

    // if(__get_node_type(pos->unitType) != CLOSE_LEAF_SIGN){
    //     ((NodeBindingCb)pos->cb)(pos);
    // }
}



void enterExit_to_newPage(curHandle_Typedef *handle, u8_t mode)
{
    MenuItem_Typedef *pos;
    u8_t cnt = 0;
    u8_t cur_page_id = getCurPage();
    struct single_list_head *ptr = &(nodelist[cur_page_id]->localPos);
    if(mode == ENTER_PAGE){

        if(ptr->next){//非空进入下一个页面
            single_list_for_each_entry(pos,ptr,brother)
            {
                if(cnt == handle->cur_choose){

                    nodelist[cur_page_id]->cursorPos = handle->cursorPos;//提前保存一下
                    nodelist[cur_page_id]->selectNum = handle->cur_choose;

                    handle->cur_choose = 0;//
                    handle->cursorPos = 0;
                    handle->startItem = 0;
                    shiftPage(pos->id);
                    break;
                }
                cnt++;
            }
        }
    }else{//返回上一级
        if(get_uplist_from_curlisthead(handle)){

        }
    }

    
}


MenuItem_Typedef* uintCreate(u8_t id , const char *text)
{
    MenuItem_Typedef* non_leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(non_leaf == NULL){
        return NULL;
    }
    memset(non_leaf,0,sizeof(MenuItem_Typedef));
    non_leaf->briefInfo = text;
    non_leaf->id = id;
    
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


void currentHandleInit(MenuItem_Typedef * base, curHandle_Typedef *handle)
{
    memset(handle,0,sizeof(curHandle_Typedef));
    handle->cur_type = base->unitType;
    handle->chosse_cnt = get_menu_choose_cnt(root);
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


