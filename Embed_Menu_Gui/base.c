#include "../Common_Struct_lib/Hi_single_list.h"
#include "../Common_Struct_lib/Hi_Queue.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "time.h"

#define True    1
#define False   0

#define PAGE_NUMS   3   //一页最多容纳 3 行

typedef struct MenuItem MenuItem_Typedef;

typedef void (*show_dir_page)(const MenuItem_Typedef *menu);

typedef void (*show_leaf_page)( MenuItem_Typedef *leaf);

typedef void (*leaf_page_keyDeal)(void);


typedef struct iconInfo
{
    const char *on_icon;
    const char *off_icon;
}iconInfo_Typedef;
typedef struct MenuItem
{
    u8_t unitType;          //该菜单节点的信息

    char selectNum;//选中的条目序号
    char cursorPos;
    const char *briefInfo;  //子菜单标题信息
    const iconInfo_Typedef *icon;       //子菜单的图标信息
    const char *cur_icon;
    struct single_list_head  localPos;  //绑定子目录的头节点
    struct single_list_head  brother;
    struct single_list_head *parentPtr;
    union{
        show_dir_page   showMenu;
        show_leaf_page  endPageDeal;    //包括 显示静态/动态页面和全局配置修改后调用上一次刷新配置页面
    };
}MenuItem_Typedef;




typedef struct configSet
{
    u8_t need_refresh;//是否需要刷屏
    u8_t bt_state;//蓝牙开关状态
    u8_t correct_state;//自动改正开关
    u8_t oneHandle_state;//单手状态
    u8_t glid_state;//是否允许滑动

}configSet_Typedef;


configSet_Typedef *operat_config;

time_t *timep;

#define LEAF_TYPE_BIT   (6)
#define LEAF_MUTLI_BIT  (5)
#define LEAF_STATE_BIT  (4)


#define LEAF_INIT_STATE     (1 << LEAF_STATE_BIT)
#define MULTI_LEAF_ASSERT   (1 << LEAF_MUTLI_BIT)
#define CAN_ENTER_ASSERT    (3 << LEAF_TYPE_BIT)

/**
 * 位值代表
 * 0叶子/1非叶子  1展开/0不能展开    1多选/0单选  默认状态1开/0关
*/

enum NODE_TYPE{
    NON_LEAF = 0x80,//非叶子节点
    LEAF_OPEN = 0x40 ,//可以展开的叶子节点
    LEAF_CLOSE_MULTI_DISEN = 0x20,//不能展开的叶子节点, 支持多选, 默认状态是关
    LEAF_CLOSE_MULTI_EN = 0x30,//不能展开的叶子节点, 支持多选, 默认状态是开
    LEAF_CLOSE_NOMULTI_DISEN = 0,//不能展开的叶子节点, 不支持多选, 默认状态是关
    LEAF_CLOSE_NOMULTI_EN   = 0x10,//不能展开的叶子节点, 不支持多选, 默认状态是开
};

enum{
    CLOSE_LEAF_SIGN = 0,//也即原来的叶子节点,如果是这种节点,不要进入下一页
    OPEN_LEAF_SIGN = 1,
    NON_LEAF_SIGN = 2,
};

enum{
    ENTER_PAGE,
    RETURN_PAGE,
};

struct cur_indicate
{
    u8_t cur_type;
    u8_t chosse_cnt;
    char cur_choose;

    char startItem;//顶叶序号
    char cursorPos;//光标位置
    u8_t show_cnt;//显示的目数
    struct single_list_head *cur_list_head;//指向菜单的头节点
};

struct cur_indicate cur_mode;


//定义开关的风格
iconInfo_Typedef text_onoff = {
    .on_icon = "开",
    .off_icon = "关",
};

iconInfo_Typedef sign_onoff = {
    .on_icon = "√",
    .off_icon = " ",
};



static inline u8_t __get_node_type(u8_t multi)
{
    return (multi>>LEAF_TYPE_BIT)&3;
}




//一种就是这种简单的只显示菜单项的方式
void simulate_show_list_page(const MenuItem_Typedef *menu)//由非叶子节点调用，显示子页面内容
{
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    u8_t cnt = 0;
    u8_t labelNum = 0;
    printf("======%s======\t\n",menu->briefInfo);
    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt >= cur_mode.startItem){
            if(labelNum == cur_mode.cursorPos){
                printf("==>:%s\t\n",temp->briefInfo);
            }else{
                printf("    %s\t\n",temp->briefInfo);
            }
            labelNum++;
            if(labelNum >= PAGE_NUMS){
                break;
            }
        }
        cnt++;
    }

    while (labelNum < PAGE_NUMS)
    {
        labelNum++;
        printf("\n");
    }
    
    printf("================\t\n");
}



void simulate_show_option_icon(const MenuItem_Typedef *menu)
{
    //这个页面由具有选择项子页面的非叶子节点调用
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    u8_t cnt = 0;
    printf("======%s======\t\n",menu->briefInfo);
    single_list_for_each_entry(temp,list_node,brother)
    {
        if(cnt == cur_mode.cur_choose)
            printf("==>:%s           %s\t\n",temp->briefInfo,temp->cur_icon);//有问题
        else
            printf("    %s           %s\t\n",temp->briefInfo,temp->cur_icon);
        
        cnt++;
    }
    while (cnt < PAGE_NUMS)
    {
        cnt++;
        printf("\n");
    }
    printf("================\t\n");
}






/**
 * 
 * 以下仅为界面的测试,与核心无关
*/

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
    printf("======%s======\n",leaf->briefInfo);
    printf("[名称:      蝴蝶与猫]\n");
    printf("[软件版本:  14.5.1]\n");
    printf("[运营商:    中国电信]\n");
    printf("====================\n");
}

void test_turn_page(MenuItem_Typedef *leaf)
{
    printf("======%s======\n",leaf->briefInfo);
    printf("[翻页测试:     .... ]\n");
    printf("====================\n");
}

void show_dynamic_time_page(MenuItem_Typedef *leaf)
{
    printf("======%s======\n",leaf->briefInfo);
    time(timep);
    char *s = ctime(timep);
    printf("%s",s);
    printf("====================\n");
    
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


void oneHandle_page_deal( MenuItem_Typedef *leaf)//这种情况是3选一
{
    operat_config->oneHandle_state = (++operat_config->oneHandle_state)%3;
    operat_config->need_refresh = 1;
    //只能单选

    leaf->cur_icon = leaf->icon->on_icon; 
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



u8_t get_menu_choose_cnt()
{
    u8_t cnt=0;
    struct single_list_head* temp = cur_mode.cur_list_head->next;
    while(temp){
        cnt++;
        temp = temp->next;
    }
    return cnt;
}

u8_t get_uplist_from_curlisthead(struct cur_indicate *curmode)
{
    MenuItem_Typedef *pos;
    struct single_list_head *ptr = curmode->cur_list_head;
    pos = list_entry(ptr,MenuItem_Typedef,localPos);

    if(pos->parentPtr == NULL){
        return False;
    }
    curmode->cur_list_head = pos->parentPtr;
    curmode->chosse_cnt = get_menu_choose_cnt();//注意摆放的位置

    // printf("返回上一层现在的选择:%d,光标:%d,开始条目:%d\n",cur_mode.cur_choose,cur_mode.cursorPos,cur_mode.startItem);
    return True;
}




void currentFace_refresh(u8_t update)
{
    MenuItem_Typedef *pos;
    struct single_list_head *ptr = cur_mode.cur_list_head;

    system("clear");
    pos = list_entry(ptr,MenuItem_Typedef,localPos);

    if(update){
        cur_mode.cur_choose = pos->selectNum;//注意摆放的位置
        cur_mode.cursorPos = pos->cursorPos;
        cur_mode.startItem = pos->selectNum - pos->cursorPos;
    }//从下层返回的刷新,先拿到保存的数据
    
    cur_mode.cur_type = pos->unitType;
    if(__get_node_type(pos->unitType) == OPEN_LEAF_SIGN){
        pos->endPageDeal(pos);
    }else if(__get_node_type(pos->unitType) == NON_LEAF_SIGN){
        pos->showMenu(pos);
    }
}

void select_verify_deal(struct cur_indicate *cur)
{
    MenuItem_Typedef *pos;
    u8_t cnt = 0;
    struct single_list_head *ptr = cur->cur_list_head;

    single_list_for_each_entry(pos,ptr,brother)
    {
        if(__get_node_type(pos->unitType) != CLOSE_LEAF_SIGN){
                return;//
        }
        if(cnt == cur->cur_choose){
            
            pos->endPageDeal(pos);//改变配置参数

            if(pos->unitType&MULTI_LEAF_ASSERT)//如果支持多选,可以直接跳出去了
                break;
        }else{
            if(!(pos->unitType&MULTI_LEAF_ASSERT)){//不支持多选
                printf("%s\n",pos->briefInfo);
                pos->cur_icon = pos->icon->off_icon;
            }
        }
        cnt++;
    }

}

void enterExit_to_newPage(struct cur_indicate *cur, u8_t mode)
{
    MenuItem_Typedef *pos,*save;
    u8_t cnt = 0;
    struct single_list_head *ptr = cur->cur_list_head;
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
                if(cnt == cur->cur_choose && __get_node_type(pos->unitType) != CLOSE_LEAF_SIGN){

                    save->cursorPos = cur->cursorPos;//提前保存一下
                    save->selectNum = cur->cur_choose;

                    cur->cur_choose = 0;//注意摆放的位置
                    cur->cursorPos = 0;
                    cur->startItem = 0;
                    cur->cur_list_head = &pos->localPos;//重新初始list
                    
                    cur->chosse_cnt = get_menu_choose_cnt();//注意摆放的位置
                    break;
                }
                cnt++;
            }
        }
    currentFace_refresh(0);
    }else{//返回上一级
        get_uplist_from_curlisthead(cur);
        currentFace_refresh(1);
    }

    
}



MenuItem_Typedef* branch_create(u8_t nodeType , const char *text, show_dir_page cb)
{
    MenuItem_Typedef* non_leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(non_leaf == NULL){
        return NULL;
    }
    memset(non_leaf,0,sizeof(MenuItem_Typedef));
    non_leaf->briefInfo = text;
    non_leaf->unitType = nodeType;
    non_leaf->showMenu = cb;
    

    return non_leaf;
} 

MenuItem_Typedef* leaf_create(u8_t nodeType, const char *text, show_leaf_page cb , iconInfo_Typedef *argIcon)
{
    MenuItem_Typedef* leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(leaf == NULL){
        return NULL;
    }
    memset(leaf,0,sizeof(MenuItem_Typedef));
    leaf->endPageDeal = cb;
    leaf->icon = argIcon;
    leaf->unitType = nodeType;
    leaf->briefInfo = text;

    if(argIcon){
        if(nodeType&LEAF_INIT_STATE){
            leaf->cur_icon = argIcon->on_icon;
        }else{
            leaf->cur_icon = argIcon->off_icon;
        }

    }

    return leaf;
}



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


/**
 * 测试菜单类型
 * 
 * 1. 需要再检查代表当前的cur_mode类型简化，能不能不独立出这个单独类型
 * 
 * TODO:
 * 简化重构当前的程序
 * 增加一页显示固定长度的功能
 * 
 * 增加从新页退出后恢复原来光标的位置
 * 
*/




void main()
{
    MenuItem_Typedef *rootNode;
    MenuItem_Typedef *UniversalNode;
    MenuItem_Typedef *BluetoothNode;
    MenuItem_Typedef *PhoneNode;
    MenuItem_Typedef *TimeNode;
    MenuItem_Typedef *KeyNode;
    MenuItem_Typedef *CorrectNode;
    MenuItem_Typedef *oneHandleNode;
    MenuItem_Typedef *slideInputNode;

    MenuItem_Typedef *BluetoothNode_1, *CorrectNode_1, *slideInputNode_1, *oneHandleNode_1, *oneHandleNode_2, *oneHandleNode_3;

    MenuItem_Typedef *NotifyNode, *HotsportNode, *NoDisturbNode, *WifiNode;

    u8_t cmd;

    MenuItem_Typedef *deal_special_page;

    timep = malloc(sizeof(*timep));
    

    operat_config = (configSet_Typedef*)malloc(sizeof(configSet_Typedef));
    memset(operat_config, 0, sizeof(configSet_Typedef));
    
    rootNode = branch_create(NON_LEAF,"设置",simulate_show_list_page);
    UniversalNode = branch_create(NON_LEAF,"通用",simulate_show_list_page);
    KeyNode = branch_create(NON_LEAF,"键盘",simulate_show_list_page);

    BluetoothNode = branch_create(NON_LEAF,"蓝牙",simulate_show_option_icon);//增加蓝牙开关控制节点

    //添加,翻页测试使用
    WifiNode = leaf_create(LEAF_OPEN, "WIFI",test_turn_page,NULL);//静态显示的
    NotifyNode = leaf_create(LEAF_OPEN, "通知",test_turn_page,NULL);//静态显示的
    HotsportNode = leaf_create(LEAF_OPEN, "个人热点",test_turn_page,NULL);//静态显示的
    NoDisturbNode = leaf_create(LEAF_OPEN, "勿扰模式",test_turn_page,NULL);//静态显示的
    /////////////

    PhoneNode = leaf_create(LEAF_OPEN, "关于本机",aboutPhone_page,NULL);//静态显示的
    TimeNode = leaf_create(LEAF_OPEN,"时间",show_dynamic_time_page,NULL);//动态显示

    CorrectNode = branch_create(NON_LEAF,"自动改正",simulate_show_option_icon);//增加开关控制节点
    oneHandleNode = branch_create(NON_LEAF,"单手键盘",simulate_show_option_icon);//增加开关控制节点
    slideInputNode = branch_create(NON_LEAF,"滑行键入",simulate_show_option_icon);



    
    BluetoothNode_1 = leaf_create(LEAF_CLOSE_MULTI_DISEN, "蓝牙",blueTooth_page_deal, &text_onoff);//增加蓝牙开关控制节点 
    CorrectNode_1 = leaf_create(LEAF_CLOSE_MULTI_DISEN, "自动改正",autoCorrct_page_deal, &text_onoff);
    slideInputNode_1 = leaf_create(LEAF_CLOSE_MULTI_DISEN, "滑行键入",glide_page_deal,&text_onoff);
    oneHandleNode_1 = leaf_create(LEAF_CLOSE_NOMULTI_EN, "左",oneHandle_page_deal, &sign_onoff);
    oneHandleNode_2 = leaf_create(LEAF_CLOSE_NOMULTI_DISEN, "中",oneHandle_page_deal, &sign_onoff);
    oneHandleNode_3 = leaf_create(LEAF_CLOSE_NOMULTI_DISEN, "右",oneHandle_page_deal, &sign_onoff);





    tree_node_binding_oneTime(7, rootNode,BluetoothNode,WifiNode,UniversalNode,NotifyNode,HotsportNode,NoDisturbNode,TimeNode);

    tree_node_binding_oneTime(2, UniversalNode,PhoneNode,KeyNode);

    tree_node_binding_oneTime(3, KeyNode,CorrectNode,
                                oneHandleNode,slideInputNode);

    tree_node_binding_oneTime(1,BluetoothNode,BluetoothNode_1);
    tree_node_binding_oneTime(1,CorrectNode,CorrectNode_1);
    tree_node_binding_oneTime(1,slideInputNode,slideInputNode_1);
    tree_node_binding_oneTime(3,oneHandleNode,oneHandleNode_1,oneHandleNode_2,oneHandleNode_3);

    cur_mode.cur_list_head = &rootNode->localPos;
    cur_mode.cur_type = rootNode->unitType;
    cur_mode.cur_choose = 0;
    cur_mode.chosse_cnt = get_menu_choose_cnt();
    cur_mode.startItem = 0;
    cur_mode.cur_type = 0;


    currentFace_refresh(0);
    

    while(1)
    {
        cmd = getchar();
        system("stty echo");
        switch (cmd)
        {
        case 'w'://光标向上
            if(__get_node_type(cur_mode.cur_type) == NON_LEAF_SIGN)
            {
                cur_mode.cur_choose--;
                if(cur_mode.cur_choose<0)
                {
                    cur_mode.cur_choose = 0;
                }

                cur_mode.cursorPos--;
                if(cur_mode.chosse_cnt <= PAGE_NUMS){
                    cur_mode.show_cnt = cur_mode.chosse_cnt;
                    if(cur_mode.cursorPos < 0){
                        cur_mode.cursorPos = 0;//diff
                    }
                }else{
                    cur_mode.show_cnt = PAGE_NUMS;
                    if(cur_mode.cursorPos < 0){
                        cur_mode.cursorPos = 0;
                        if(cur_mode.startItem > 0 ){
                            cur_mode.startItem--;
                        }
                    }
                }
                // printf("现在的选择:%d,光标:%d,开始条目:%d\n",cur_mode.cur_choose,cur_mode.cursorPos,cur_mode.startItem);



                currentFace_refresh(0);
            }
            break;
        case 'a'://返回
            enterExit_to_newPage(&cur_mode,RETURN_PAGE);
            break;
        case 's'://光标向下
            if(__get_node_type(cur_mode.cur_type) == NON_LEAF_SIGN)
            {
                cur_mode.cur_choose++;
                if(cur_mode.cur_choose >= cur_mode.chosse_cnt){
                    cur_mode.cur_choose = cur_mode.chosse_cnt-1;
                }


                cur_mode.cursorPos++;
                if(cur_mode.chosse_cnt <= PAGE_NUMS){
                    cur_mode.show_cnt = cur_mode.chosse_cnt;
                    if(cur_mode.cursorPos >= cur_mode.show_cnt){
                        cur_mode.cursorPos = cur_mode.show_cnt-1;//diff
                    }
                }else{
                    cur_mode.show_cnt = PAGE_NUMS;
                    if(cur_mode.cursorPos >= PAGE_NUMS){
                        cur_mode.cursorPos = PAGE_NUMS-1;
                        if(cur_mode.startItem < cur_mode.chosse_cnt - PAGE_NUMS){
                            cur_mode.startItem++;
                        }
                    }
                }
                // printf("现在的选择:%d,光标:%d,开始条目:%d\n",cur_mode.cur_choose,cur_mode.cursorPos,cur_mode.startItem);
                currentFace_refresh(0);
            }
            break;
        case 'd'://进入 
            enterExit_to_newPage(&cur_mode,ENTER_PAGE);
            break;
        case 'e':
            //1 执行对应deal函数
            //2 刷新当前FALSE_NON_LEAF页面
            select_verify_deal(&cur_mode);
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
            currentFace_refresh(0);
        }


    }

    free_branch_auto(rootNode);
    free(operat_config);
    free(timep);
}












