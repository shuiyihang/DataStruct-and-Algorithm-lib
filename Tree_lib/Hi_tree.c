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

typedef void (*show_leaf_page)(const MenuItem_Typedef *leaf);
typedef struct MenuItem
{
    unsigned char type;
    const char *brief_info;
    struct single_list_head  localPos;//绑定子目录的头节点
    struct single_list_head  brother;
    struct single_list_head *parentPtr;
    union{
        show_dir_page   showMenu;
        show_leaf_page  showPage;
    };
}MenuItem_Typedef;

enum{
    LEAF=1,
    NON_LEAF,
};

enum{
    ENTER_PAGE,
    RETURN_PAGE,
};

struct cur_indicate
{
    unsigned char cur_type;
    unsigned char chosse_cnt;
    char cur_choose;
    struct single_list_head *cur_list_head;//指向菜单的头节点
};

struct cur_indicate cur_mode;

void simulate_show_leaf_page(const MenuItem_Typedef *leaf)
{
    printf("================\n");
    printf("[   %s  ]\n",leaf->brief_info);
    printf("================\n");
}

//只做显示处理,并不希望改变
void simulate_show_list_page(const MenuItem_Typedef *menu)//由非叶子节点调用，显示子页面内容
{
    const struct single_list_head *list_node = &menu->localPos;
    MenuItem_Typedef *temp;
    unsigned char cnt = 0;
    printf("====%s====\t\n",menu->brief_info);
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

    pos = list_entry(ptr,MenuItem_Typedef,localPos);
    if(pos->type == LEAF){
        cur_mode.cur_type = LEAF;
        pos->showPage(pos);
    }else if(pos->type == NON_LEAF){
        cur_mode.cur_type = NON_LEAF;
        pos->showMenu(pos);
    }
}


void enter_return_new_page(struct cur_indicate *cur, unsigned char mode)
{
    MenuItem_Typedef *pos;
    unsigned char cnt = 0;
    struct single_list_head *ptr = cur->cur_list_head;
    if(mode == ENTER_PAGE){
        single_list_for_each_entry(pos,ptr,brother)
        {
            if(cnt == cur->cur_choose){
                cur->cur_choose = 0;//注意摆放的位置
                cur->cur_list_head = &pos->localPos;//重新初始list
                
                cur->chosse_cnt = get_menu_choose_cnt();//注意摆放的位置
                break;
            }
            cnt++;
        }
    }else{//返回上一级
        get_uplist_from_curlisthead(cur);
    }

    refresh_cur_interface();
}



MenuItem_Typedef* non_leaf_create(const char *text, show_dir_page cb)
{
    MenuItem_Typedef* non_leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(non_leaf == NULL){
        return NULL;
    }
    memset(non_leaf,0,sizeof(MenuItem_Typedef));
    non_leaf->brief_info = text;
    non_leaf->type = NON_LEAF;
    non_leaf->showMenu = cb;

    return non_leaf;
} 

MenuItem_Typedef* leaf_create(const char *text, show_leaf_page cb)
{
    MenuItem_Typedef* leaf = (MenuItem_Typedef*)malloc(sizeof(MenuItem_Typedef));
    if(leaf == NULL){
        return NULL;
    }
    memset(leaf,0,sizeof(MenuItem_Typedef));
    leaf->showPage = cb;
    leaf->type = LEAF;
    leaf->brief_info = text;

    return leaf;
}



void free_branch_auto(MenuItem_Typedef* non_lef)
{
    struct single_list_head *ptr = &non_lef->localPos;
    MenuItem_Typedef *temp;
    
    single_list_for_each_entry(temp,ptr,brother){
        if(temp->type == LEAF){
            printf("free leaf:%s\n",temp->brief_info);
            free(temp);
        }else{
            free_branch_auto(temp);
        }
    }
    printf("free non_leaf:%s\n",non_lef->brief_info);
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
    MenuItem_Typedef *root;
    MenuItem_Typedef *first_level_one;
    MenuItem_Typedef *first_level_two;
    MenuItem_Typedef *second_level_one;
    MenuItem_Typedef *second_level_two;
    MenuItem_Typedef *third_level_one;
    MenuItem_Typedef *third_level_two;
    MenuItem_Typedef *third_level_three;
    unsigned char cmd;
    

    
    root = non_leaf_create("menu root",simulate_show_list_page);
    first_level_one = non_leaf_create("first_level_one",simulate_show_list_page);
    second_level_two = non_leaf_create("second_level_two",simulate_show_list_page);

    first_level_two = leaf_create("01",simulate_show_leaf_page);
    second_level_one = leaf_create("000",simulate_show_leaf_page);
    third_level_one = leaf_create("0011",simulate_show_leaf_page);
    third_level_two = leaf_create("0000",simulate_show_leaf_page);
    third_level_three = leaf_create("0002",simulate_show_leaf_page);


    tree_node_binding_oneTime(2, root,first_level_one,first_level_two);

    tree_node_binding_oneTime(2, first_level_one,second_level_one,second_level_two);

    tree_node_binding_oneTime(3, second_level_two,third_level_one,
                                third_level_two,third_level_three);

    cur_mode.cur_list_head = &root->localPos;
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
            if(cur_mode.cur_type == NON_LEAF)
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
            if(cur_mode.cur_type == NON_LEAF)
            {
                cur_mode.cur_choose = (++cur_mode.cur_choose)%cur_mode.chosse_cnt;
                refresh_cur_interface();
            }
            break;
        case 'd'://进入
            enter_return_new_page(&cur_mode,ENTER_PAGE);
            break;
        default:
            break;
        }
        if(cmd == 'q'){
            break;
        }
        cmd = 'l';


    }

    free_branch_auto(root);
}









// 一个节点如果不是叶子节点，就把他所有孩子的文字信息，以列表方式列出

















//前序非递归--根左右--利用栈
// void pre_order_visit(struct btnode *root)
// {
//     if(root != NULL)
//     {
//         struct btnode *_stack[bt_max_size];
//         int top = -1;
//         struct btnode *_temp = NULL;
//         _stack[++top] = root;
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

