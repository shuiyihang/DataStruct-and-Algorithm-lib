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

typedef struct leaf_accept menu_non_leaf;
typedef struct leaf_node menu_leaf;

typedef void (*show_dir_page)(const menu_non_leaf *menu);

typedef void (*show_leaf_page)(const menu_leaf *leaf);
typedef struct node_type_name
{
    unsigned char type;
    struct single_list_head local_pos;//绑定子目录的头节点
    struct single_list_head hook;
    struct single_list_head *parentPtr;
}node_type_name;

typedef struct leaf_accept
{
    const char *brief_info;
    node_type_name page_type;
    show_dir_page page_deal;
}menu_non_leaf;

typedef struct leaf_node
{
    const char *brief_info;
    node_type_name page_type;
    show_leaf_page page_deal;
}menu_leaf;

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
    struct single_list_head *cur_list_head;
};

struct cur_indicate cur_mode;

void simulate_show_leaf_page(const menu_leaf *leaf)
{
    printf("================\n");
    printf("[   %s  ]\n",leaf->brief_info);
    printf("================\n");
}

//只做显示处理,并不希望有人改变
void simulate_show_list_page(const menu_non_leaf *menu)//由非叶子节点调用，显示子页面内容
{
    const struct single_list_head *list_node = &menu->page_type.local_pos;
    node_type_name *temp;
    menu_leaf *leaf_ui;
    menu_non_leaf *contain_ui;
    unsigned char cnt = 0;
    printf("========%s========\t\n",menu->brief_info);
    single_list_for_each_entry(temp,list_node,hook)
    {
        if(temp->type == LEAF){
            leaf_ui = list_entry(temp,menu_leaf,page_type);
            if(cnt == cur_mode.cur_choose)
            printf("==>:%s\t\n",leaf_ui->brief_info);
            else
            printf("    %s\t\n",leaf_ui->brief_info);
        }else if(temp->type == NON_LEAF){
            contain_ui = list_entry(temp,menu_non_leaf,page_type);
            if(cnt == cur_mode.cur_choose)
            printf("==>:%s\t\n",contain_ui->brief_info);
            else
            printf("    %s\t\n",contain_ui->brief_info);
        }
        cnt++;
    }
    printf("================\t\n");
}


//建树
//通过指定父亲和儿子来构件关系

void tree_node_bingding_by_ps(node_type_name *non_leaf,node_type_name *leaf)
{
    single_list_add_tail(&leaf->hook,&non_leaf->local_pos);
    leaf->parentPtr = &non_leaf->local_pos;
}



unsigned char get_menu_choose_cnt(struct single_list_head *ptr)
{
    unsigned char cnt=0;
    struct single_list_head* temp = ptr->next;
    while(temp){
        cnt++;
        temp = temp->next;
    }
    return cnt;
}

unsigned char get_uplist_from_curlisthead(struct cur_indicate *curmode)
{
    node_type_name *pos;
    menu_non_leaf *contain_ui;
    menu_leaf *leaf_ui;
    struct single_list_head *ptr = curmode->cur_list_head;
    pos = list_entry(ptr,node_type_name,local_pos);

    if(pos->parentPtr == NULL){
        return False;
    }
    if(pos->type == LEAF){
        leaf_ui = list_entry(pos,menu_leaf,page_type);
        curmode->cur_list_head = leaf_ui->page_type.parentPtr;//更新当前界面的指向
    }else{
        contain_ui = list_entry(pos,menu_non_leaf,page_type);
        curmode->cur_list_head = contain_ui->page_type.parentPtr;
    }
    return True;
}




void refresh_cur_interface(void)
{
    node_type_name *pos;
    menu_leaf *leaf_ui;
    menu_non_leaf *contain_ui;
    struct single_list_head *ptr = cur_mode.cur_list_head;

    pos = list_entry(ptr,node_type_name,local_pos);
    if(pos->type == LEAF){
        leaf_ui = list_entry(pos,menu_leaf,page_type);
        leaf_ui->page_deal(leaf_ui);
    }else if(pos->type == NON_LEAF){
        contain_ui = list_entry(pos,menu_non_leaf,page_type);
        contain_ui->page_deal(contain_ui);
    }
}


void enter_return_new_page(struct cur_indicate *cur, unsigned char mode)
{
    node_type_name *pos;
    unsigned char cnt = 0;
    menu_leaf *leaf_ui;
    menu_non_leaf *contain_ui;
    struct single_list_head *ptr = cur->cur_list_head;
    if(mode == ENTER_PAGE){
        single_list_for_each_entry(pos,ptr,hook)
        {
            if(cnt == cur->cur_choose){
                cur_mode.cur_choose = 0;//注意摆放的位置
                if(pos->type == LEAF){
                    leaf_ui = list_entry(pos,menu_leaf,page_type);
                    leaf_ui->page_deal(leaf_ui);//这里要画图，所以要提前清零choose
                    cur_mode.cur_type = LEAF;
                    cur_mode.cur_list_head = &leaf_ui->page_type.local_pos;//重新初始list
                }else if(pos->type == NON_LEAF){
                    contain_ui = list_entry(pos,menu_non_leaf,page_type);
                    contain_ui->page_deal(contain_ui);
                    cur_mode.cur_type = NON_LEAF;
                    cur_mode.cur_list_head = &contain_ui->page_type.local_pos;
                }
                cur->chosse_cnt = get_menu_choose_cnt(cur_mode.cur_list_head);//注意摆放的位置
                break;
            }
            cnt++;
        }
    }else{//返回上一级
    
        if(get_uplist_from_curlisthead(&cur_mode) == True)
        {
            cur_mode.cur_type = NON_LEAF;
            cur_mode.cur_choose = 0;//注意摆放的位置
            refresh_cur_interface();
            cur->chosse_cnt = get_menu_choose_cnt(cur_mode.cur_list_head);//注意摆放的位置
        }
    }
}



menu_non_leaf* non_leaf_create(const char *text, show_dir_page cb)
{
    menu_non_leaf* non_leaf = (menu_non_leaf*)malloc(sizeof(menu_non_leaf));
    if(non_leaf == NULL){
        return NULL;
    }
    memset(non_leaf,0,sizeof(menu_non_leaf));
    non_leaf->brief_info = text;
    non_leaf->page_type.type = NON_LEAF;
    non_leaf->page_deal = cb;
    non_leaf->page_type.parentPtr = NULL;

    return non_leaf;
} 

menu_leaf* leaf_create(const char *text, show_leaf_page cb)
{
    menu_leaf* leaf = (menu_leaf*)malloc(sizeof(menu_leaf));
    if(leaf == NULL){
        return NULL;
    }
    memset(leaf,0,sizeof(menu_leaf));
    leaf->page_deal = cb;
    leaf->page_type.type = LEAF;
    leaf->brief_info = text;

    return leaf;
}



void free_branch_auto(menu_non_leaf* non_lef)
{
    struct single_list_head *ptr = &non_lef->page_type.local_pos;
    node_type_name *temp;
    menu_leaf *leaf_ui;
    menu_non_leaf *contain_ui;
    
    single_list_for_each_entry(temp,ptr,hook){
        if(temp->type == LEAF){
            leaf_ui = list_entry(temp,menu_leaf,page_type);
            printf("free leaf:%s\n",leaf_ui->brief_info);
            free(leaf_ui);
        }else{
            contain_ui = list_entry(temp,menu_non_leaf,page_type);
            free_branch_auto(contain_ui);
        }
    }
    printf("free non_leaf:%s\n",non_lef->brief_info);
    free(non_lef);
}


/**
 * 测试菜单类型
 * 
*/


void main()
{
    menu_non_leaf *root;
    menu_non_leaf *first_level_one;
    menu_leaf *first_level_two;
    menu_leaf *second_level_one;
    menu_non_leaf *second_level_two;
    menu_leaf *third_level_one;
    menu_leaf *third_level_two;
    menu_leaf *third_level_three;
    unsigned char cmd;
    

    
    root = non_leaf_create("menu root",simulate_show_list_page);
    first_level_one = non_leaf_create("first_level_one",simulate_show_list_page);
    second_level_two = non_leaf_create("second_level_two",simulate_show_list_page);

    first_level_two = leaf_create("01",simulate_show_leaf_page);
    second_level_one = leaf_create("000",simulate_show_leaf_page);
    third_level_one = leaf_create("0011",simulate_show_leaf_page);
    third_level_two = leaf_create("0000",simulate_show_leaf_page);
    third_level_three = leaf_create("0002",simulate_show_leaf_page);


    tree_node_bingding_by_ps(&root->page_type,&first_level_one->page_type);
    tree_node_bingding_by_ps(&root->page_type,&first_level_two->page_type);

    

    tree_node_bingding_by_ps(&first_level_one->page_type,&second_level_one->page_type);
    tree_node_bingding_by_ps(&first_level_one->page_type,&second_level_two->page_type);

    tree_node_bingding_by_ps(&second_level_two->page_type,&third_level_one->page_type);
    tree_node_bingding_by_ps(&second_level_two->page_type,&third_level_two->page_type);
    tree_node_bingding_by_ps(&second_level_two->page_type,&third_level_three->page_type);

    cur_mode.cur_list_head = &root->page_type.local_pos;
    cur_mode.cur_type = NON_LEAF;
    cur_mode.cur_choose = 0;
    cur_mode.chosse_cnt = get_menu_choose_cnt(cur_mode.cur_list_head);


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

