/**
 * 先实现各种树结构吧
 * 左孩子是子节点，右孩子是兄弟节点，常规树转二叉树
 * 
 * 也是有一个根节点
*/
#include "../Common_Struct_lib/Hi_single_list.h"
#include "../Common_Struct_lib/public.h"

#include "../Common_Struct_lib/Hi_Queue.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#define True    1
#define False   0
struct btnode
{
    char data;
    struct single_list_head lchild;
    struct single_list_head rchild;
};

//表头结构类型
typedef struct
{
    struct single_list_head parents;//指向双亲
    struct single_list_head child;//指向第一个孩子

}tree_node_info;

#define bt_max_size     10
//一些遍历的demo

typedef struct MenuItem MenuItem_Typedef;

typedef void (*show_dir_page)(const MenuItem_Typedef *menu);

typedef void (*show_leaf_page)( MenuItem_Typedef *leaf);

typedef void (*leaf_page_keyDeal)(void);


typedef struct iconInfo
{
    char *on_icon;
    char *off_icon;
    // unsigned char set_x;
    // unsigned char set_y;
}iconInfo_Typedef;
typedef struct MenuItem
{
    unsigned char type;
    const char *brief_info;//子菜单标题信息
    iconInfo_Typedef *icon;//子菜单的图标信息
    char *cur_icon;
    struct single_list_head  localPos;//绑定子目录的头节点
    struct single_list_head  brother;
    struct single_list_head *parentPtr;
    union{
        show_dir_page   showMenu;
        show_leaf_page  showPage;
    };
    unsigned char multiOne;//子节点只支持多选一
}MenuItem_Typedef;




typedef struct configSet
{
    unsigned char need_refresh;//是否需要刷屏
    unsigned char bt_state;//蓝牙开关状态
    unsigned char correct_state;//自动改正开关
    unsigned char oneHandle_state;//单手状态
    unsigned char glid_state;//是否允许滑动

}configSet_Typedef;


configSet_Typedef *operat_config;




enum{
    OPEN_LEAF=1,
    CLOSE_LEAF,//也即原来的叶子节点,如果是这种节点,不要进入下一页
    NON_LEAF,
};

enum{
    ENTER_PAGE,
    RETURN_PAGE,
};

enum{
    STATE_NULL,//只是显示界面
    STATE_ON,
    STATE_OFF,
};

enum{
    MULTI_SUPPORT,
    NON_MULTI_SUPPORT,
};
struct cur_indicate
{
    unsigned char cur_type;
    unsigned char chosse_cnt;
    char cur_choose;
    struct single_list_head *cur_list_head;//指向菜单的头节点
};

struct cur_indicate cur_mode;


//另一种除了显示菜单项，在右边有对应图标指示，开/关,滑动按钮,对勾等
//进入这个页面之后,有选中项,点击确定之后,更改配置值,同时刷新这个页面显示
//也就是四个按键,上下滑动页面,左右上一级下一级,确定只有在子页面时候生效,用来改变配置值


iconInfo_Typedef text_onoff = {
    .on_icon = "开",
    .off_icon = "关",
    // .cur_icon = text_off_icon,
};

iconInfo_Typedef sign_onoff = {
    .on_icon = "√",
    .off_icon = " ",
    // .cur_icon = sign_off_icon,
};














void simulate_show_leaf_page(const MenuItem_Typedef *leaf)
{
    printf("================\n");
    printf("[   %s  ]\n",leaf->brief_info);
    printf("================\n");
}

//一种就是这种简单的只显示菜单项的方式
void simulate_show_list_page(const MenuItem_Typedef *menu)//由非叶子节点调用，显示子页面内容
{
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    unsigned char cnt = 0;
    printf("======%s======\t\n",menu->brief_info);
    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt == cur_mode.cur_choose)
            printf("==>:%s\t\n",temp->brief_info);
        else
            printf("    %s\t\n",temp->brief_info);
        cnt++;
    }
    printf("================\t\n");
}



void simulate_show_option_icon(const MenuItem_Typedef *menu)
{
    //这个页面由具有选择项子页面的非叶子节点调用
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    unsigned char cnt = 0;
    printf("======%s======\t\n",menu->brief_info);
    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt == cur_mode.cur_choose)
            printf("==>:%s           %s\t\n",temp->brief_info,temp->cur_icon);//有问题
        else
            printf("    %s           %s\t\n",temp->brief_info,temp->cur_icon);
        
        cnt++;
    }
    printf("================\t\n");
}






/**
 * 
 * 以下仅为界面的测试,与核心无关
*/


void blueTooth_page(const MenuItem_Typedef *leaf)
{
    printf("======%s======\n",leaf->brief_info);
    if(operat_config->bt_state)
        printf("[蓝牙:         %s]\n",leaf->icon->on_icon);
    else
        printf("[蓝牙:         %s]\n",leaf->icon->off_icon);
    printf("===================\n");
}
void blueTooth_page_deal( MenuItem_Typedef *leaf)
{
    operat_config->bt_state = ~operat_config->bt_state;
    operat_config->need_refresh = 1;
    if(operat_config->bt_state){
        leaf->cur_icon = leaf->icon->on_icon;
    }else{
        leaf->cur_icon = leaf->icon->off_icon;
    }
}


void aboutPhone_page( MenuItem_Typedef *leaf)
{
    printf("======%s======\n",leaf->brief_info);
    printf("[名称:      蝴蝶与猫]\n");
    printf("[软件版本:  14.5.1]\n");
    printf("[运营商:    中国电信]\n");
    printf("====================\n");
}


void autoCorrect_page(const MenuItem_Typedef *leaf)
{
    printf("=====%s=====\n",leaf->brief_info);
    if(operat_config->correct_state)
        printf("[自动改正:      %s]\n",leaf->icon->on_icon);
    else
        printf("[自动改正:      %s]\n",leaf->icon->off_icon);
    printf("==================\n");
}
void autoCorrct_page_deal( MenuItem_Typedef *leaf)
{
    operat_config->correct_state = ~operat_config->correct_state;
    operat_config->need_refresh = 1;
    if(operat_config->correct_state){
        leaf->cur_icon = leaf->icon->on_icon;
    }else{
        leaf->cur_icon = leaf->icon->off_icon;
    }
    
}


void oneHandle_page(const MenuItem_Typedef *leaf)
{
    printf("=====%s=====\n",leaf->brief_info);

    switch (operat_config->oneHandle_state)
    {
    case 0:
        printf("[关:         %s ]\n",leaf->icon->on_icon);
        printf("[左:         %s ]\n",leaf->icon->off_icon);
        printf("[右:         %s ]\n",leaf->icon->off_icon);
        break;
    case 1:
        printf("[关:         %s ]\n",leaf->icon->off_icon);
        printf("[左:         %s ]\n",leaf->icon->on_icon);
        printf("[右:         %s ]\n",leaf->icon->off_icon);
        break;
    
    case 2:
        printf("[关:         %s ]\n",leaf->icon->off_icon);
        printf("[左:         %s ]\n",leaf->icon->off_icon);
        printf("[右:         %s ]\n",leaf->icon->on_icon);
        break;
    
    default:
        break;
    }
    printf("==================\n");
}
void oneHandle_page_deal( MenuItem_Typedef *leaf)//这种情况是3选一
{
    operat_config->oneHandle_state = (++operat_config->oneHandle_state)%3;
    operat_config->need_refresh = 1;
    //只能单选

    leaf->cur_icon = leaf->icon->on_icon; 
}


void glideType(const MenuItem_Typedef *leaf)
{
    printf("======%s======\n",leaf->brief_info);
    if(operat_config->correct_state)
        printf("[滑行键入:        %s]\n",leaf->icon->on_icon);
    else    
        printf("[滑行键入:        %s]\n",leaf->icon->off_icon);
    printf("====================\n");
}
void glide_page_deal( MenuItem_Typedef *leaf)
{
    operat_config->glid_state = ~operat_config->glid_state;
    operat_config->need_refresh = 1;
    if(operat_config->glid_state){
        leaf->cur_icon = leaf->icon->on_icon;
    }else{
        leaf->cur_icon = leaf->icon->off_icon;
    }
}











//建树
//通过指定父亲和儿子来构件关系

void tree_node_bingding_by_ps(MenuItem_Typedef *non_leaf,MenuItem_Typedef *leaf)
{
    single_list_add_tail(&leaf->brother,&non_leaf->localPos);
    leaf->parentPtr = &non_leaf->localPos;
}

//通过不定参数一次性绑定
void tree_node_binding_oneTime(int cnt, MenuItem_Typedef *non_leaf,...)
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



unsigned char get_menu_choose_cnt()
{
    unsigned char cnt=0;
    struct single_list_head* temp = cur_mode.cur_list_head->next;
    while(temp){
        cnt++;
        temp = temp->next;
    }
    return cnt;
}

unsigned char get_uplist_from_curlisthead(struct cur_indicate *curmode)
{
    MenuItem_Typedef *pos;
    struct single_list_head *ptr = curmode->cur_list_head;
    pos = list_entry(ptr,MenuItem_Typedef,localPos);

    if(pos->parentPtr == NULL){
        return False;
    }
    curmode->cur_list_head = pos->parentPtr;
    curmode->cur_choose = 0;//注意摆放的位置
    curmode->chosse_cnt = get_menu_choose_cnt();//注意摆放的位置
    return True;
}




void refresh_cur_interface(void)
{
    MenuItem_Typedef *pos;
    struct single_list_head *ptr = cur_mode.cur_list_head;

    system("clear");
    pos = list_entry(ptr,MenuItem_Typedef,localPos);
    if(pos->type == OPEN_LEAF){
        cur_mode.cur_type = OPEN_LEAF;
        pos->showPage(pos);
    }else if(pos->type == NON_LEAF){
        cur_mode.cur_type = NON_LEAF;
        pos->showMenu(pos);
    }
}

void confirm_progress(struct cur_indicate *cur)
{
    MenuItem_Typedef *pos;
    unsigned char cnt = 0;
    struct single_list_head *ptr = cur->cur_list_head;

    single_list_for_each_entry(pos,ptr,brother)
    {
        if(cnt == cur->cur_choose){
            
            if(pos->type != CLOSE_LEAF){
                return;
            }
            printf("%s\n",pos->brief_info);
            pos->showPage(pos);//改变配置参数

            if(!pos->multiOne)//如果不支持仅单选,可以直接跳出去了
                break;
        }else{
            if(pos->multiOne){
                printf("%s\n",pos->brief_info);
                pos->cur_icon = pos->icon->off_icon;
            }
        }
        cnt++;
    }
    refresh_cur_interface();

}

void enter_return_new_page(struct cur_indicate *cur, unsigned char mode)
{
    MenuItem_Typedef *pos;
    unsigned char cnt = 0;
    struct single_list_head *ptr = cur->cur_list_head;
    if(mode == ENTER_PAGE){

        pos = list_entry(ptr,MenuItem_Typedef,localPos);
        printf("%d\n",pos->type);
        if(pos->type == CLOSE_LEAF){
            printf("false non leaf\n");
            return;
        }
        if(ptr->next){//非空进入下一个页面
            single_list_for_each_entry(pos,ptr,brother)
            {
                if(cnt == cur->cur_choose && pos->type != CLOSE_LEAF){
                    cur->cur_choose = 0;//注意摆放的位置
                    cur->cur_list_head = &pos->localPos;//重新初始list
                    
                    cur->chosse_cnt = get_menu_choose_cnt();//注意摆放的位置
                    break;
                }
                cnt++;
            }
        }
        
    }else{//返回上一级
        get_uplist_from_curlisthead(cur);
    }

    refresh_cur_interface();
}



MenuItem_Typedef* non_leaf_create(unsigned char nodeType , const char *text, show_dir_page cb,  iconInfo_Typedef *argIcon)
{
    MenuItem_Typedef* non_leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(non_leaf == NULL){
        return NULL;
    }
    memset(non_leaf,0,sizeof(MenuItem_Typedef));
    non_leaf->brief_info = text;
    non_leaf->type = nodeType;
    non_leaf->showMenu = cb;
    non_leaf->icon = argIcon;
    if(argIcon){
        non_leaf->cur_icon = argIcon->off_icon;
        printf("create:%s\n",non_leaf->cur_icon);
    }
    

    return non_leaf;
} 

MenuItem_Typedef* leaf_create(unsigned char nodeType, unsigned char multi_support, const char *text, show_leaf_page cb , iconInfo_Typedef *argIcon, unsigned char initState)
{
    MenuItem_Typedef* leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(leaf == NULL){
        return NULL;
    }
    memset(leaf,0,sizeof(MenuItem_Typedef));
    leaf->showPage = cb;
    leaf->icon = argIcon;
    leaf->type = nodeType;
    leaf->brief_info = text;
    leaf->multiOne = multi_support;

    if(argIcon){
        if(initState == STATE_ON){
            leaf->cur_icon = argIcon->on_icon;
        }else if(initState == STATE_OFF){
            leaf->cur_icon = argIcon->off_icon;
        }
        
        printf("create:%s\n",leaf->cur_icon);
    }

    return leaf;
}



void free_branch_auto(MenuItem_Typedef* non_lef)
{
    struct single_list_head *ptr = &non_lef->localPos;
    MenuItem_Typedef *temp;
    
    single_list_for_each_entry(temp,ptr,brother){
        if(temp->type == OPEN_LEAF){
            // printf("free leaf:%s\n",temp->brief_info);
            free(temp);
        }else{
            free_branch_auto(temp);
        }
    }
    // printf("free non_leaf:%s\n",non_lef->brief_info);
    free(non_lef);
}


/**
 * 测试菜单类型
 * 
 * 1. 需要再检查代表当前的cur_mode类型简化，能不能不独立出这个单独类型
 * 2. 绑定时候能不能再优雅些
*/




void main()
{
    MenuItem_Typedef *rootNode;
    MenuItem_Typedef *UniversalNode;
    MenuItem_Typedef *BluetoothNode;
    MenuItem_Typedef *PhoneNode;
    MenuItem_Typedef *KeyNode;
    MenuItem_Typedef *CorrectNode;
    MenuItem_Typedef *oneHandleNode;
    MenuItem_Typedef *slideInputNode;

    MenuItem_Typedef *BluetoothNode_1, *CorrectNode_1, *slideInputNode_1, *oneHandleNode_1, *oneHandleNode_2, *oneHandleNode_3;
    unsigned char cmd;

    MenuItem_Typedef *deal_special_page;
    

    operat_config = (configSet_Typedef*)malloc(sizeof(configSet_Typedef));
    memset(operat_config, 0, sizeof(configSet_Typedef));
    
    rootNode = non_leaf_create(NON_LEAF,"设置",simulate_show_list_page,NULL);
    UniversalNode = non_leaf_create(NON_LEAF,"通用",simulate_show_list_page,NULL);
    KeyNode = non_leaf_create(NON_LEAF,"键盘",simulate_show_list_page,NULL);

    BluetoothNode = non_leaf_create(NON_LEAF,"蓝牙",simulate_show_option_icon,&text_onoff);//增加蓝牙开关控制节点
    PhoneNode = leaf_create(OPEN_LEAF,MULTI_SUPPORT, "关于本机",aboutPhone_page,NULL,STATE_NULL);
    CorrectNode = non_leaf_create(NON_LEAF,"自动改正",simulate_show_option_icon,&text_onoff);//增加开关控制节点
    oneHandleNode = non_leaf_create(NON_LEAF,"单手键盘",simulate_show_option_icon,&sign_onoff);//增加开关控制节点
    slideInputNode = non_leaf_create(NON_LEAF,"滑行键入",simulate_show_option_icon,&text_onoff);




    
    BluetoothNode_1 = leaf_create(CLOSE_LEAF, MULTI_SUPPORT, "蓝牙",blueTooth_page_deal, &text_onoff,STATE_OFF);//增加蓝牙开关控制节点 
    CorrectNode_1 = leaf_create(CLOSE_LEAF, MULTI_SUPPORT, "自动改正",autoCorrct_page_deal, &text_onoff,STATE_OFF);
    slideInputNode_1 = leaf_create(CLOSE_LEAF, MULTI_SUPPORT, "滑行键入",glide_page_deal,&text_onoff,STATE_OFF);
    oneHandleNode_1 = leaf_create(CLOSE_LEAF, NON_MULTI_SUPPORT, "左",oneHandle_page_deal, &sign_onoff,STATE_ON);
    oneHandleNode_2 = leaf_create(CLOSE_LEAF, NON_MULTI_SUPPORT, "中",oneHandle_page_deal, &sign_onoff,STATE_OFF);
    oneHandleNode_3 = leaf_create(CLOSE_LEAF, NON_MULTI_SUPPORT, "右",oneHandle_page_deal, &sign_onoff,STATE_OFF);





    tree_node_binding_oneTime(2, rootNode,UniversalNode,BluetoothNode);

    tree_node_binding_oneTime(2, UniversalNode,PhoneNode,KeyNode);

    tree_node_binding_oneTime(3, KeyNode,CorrectNode,
                                oneHandleNode,slideInputNode);

    tree_node_binding_oneTime(1,BluetoothNode,BluetoothNode_1);
    tree_node_binding_oneTime(1,CorrectNode,CorrectNode_1);
    tree_node_binding_oneTime(1,slideInputNode,slideInputNode_1);
    tree_node_binding_oneTime(3,oneHandleNode,oneHandleNode_1,oneHandleNode_2,oneHandleNode_3);

    cur_mode.cur_list_head = &rootNode->localPos;
    cur_mode.cur_type = NON_LEAF;
    cur_mode.cur_choose = 0;
    cur_mode.chosse_cnt = get_menu_choose_cnt();


    refresh_cur_interface();

    while(1)
    {
        cmd = getchar();
        system("stty echo");
        switch (cmd)
        {
        case 'w'://光标向上
            if(cur_mode.cur_type != OPEN_LEAF)
            {
                cur_mode.cur_choose--;
                if(cur_mode.cur_choose<0)
                {
                    cur_mode.cur_choose = cur_mode.chosse_cnt - 1;
                }
                refresh_cur_interface();
            }
            break;
        case 'a'://返回
            enter_return_new_page(&cur_mode,RETURN_PAGE);
            break;
        case 's'://光标向下
            if(cur_mode.cur_type != OPEN_LEAF)
            {
                cur_mode.cur_choose = (++cur_mode.cur_choose)%cur_mode.chosse_cnt;
                refresh_cur_interface();
            }
            break;
        case 'd'://进入 
            enter_return_new_page(&cur_mode,ENTER_PAGE);
            break;
        case 'e':
            //1 执行对应deal函数
            //2 刷新当前FALSE_NON_LEAF页面
            confirm_progress(&cur_mode);
            break;//确认键
        default:
            break;
        }
        if(cmd == 'q'){
            
            break;
        }
        cmd = 'l';

        if(operat_config->need_refresh){
            operat_config->need_refresh = 0;
            refresh_cur_interface();
        }


    }

    free_branch_auto(rootNode);
    free(operat_config);
}









// 一个节点如果不是叶子节点，就把他所有孩子的文字信息，以列表方式列出

















//前序非递归--根左右--利用栈
// void pre_order_visit(struct btnode *rootNode)
// {
//     if(rootNode != NULL)
//     {
//         struct btnode *_stack[bt_max_size];
//         int top = -1;
//         struct btnode *_temp = NULL;
//         _stack[++top] = rootNode;
//         while (top != -1)
//         {
//             _temp = _stack[top--];
//             printf("%c ",_temp->data);
//             if(_temp->rchild)
//                 _stack[++top] = _temp->rchild;
//             if(_temp->lchild)
//                 _stack[++top] = _temp->lchild;
//         }
        
//     }
// }
//后序非递归--左右根--

//中序非递归--左根右


//层次遍历

