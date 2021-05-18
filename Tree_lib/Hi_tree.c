/**
 * 先实现各种树结构吧
 * 左孩子是子节点，右孩子是兄弟节点，常规树转二叉树
 * 
 * 也是有一个根节点
*/
#include "../Common_Struct_lib/Hi_single_list.h"
#include "../Common_Struct_lib/public.h"
#include "stdlib.h"
#include "stdio.h"
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

typedef struct leaf_accept menu_leaf_accept;
typedef struct leaf_node menu_leaf;

typedef void (*show_dir_page)(menu_leaf_accept *menu);

typedef void (*show_leaf_page)(menu_leaf *leaf);
typedef struct node_type_name
{
    unsigned char type;
    struct single_list_head start;
    struct single_list_head hook;
    struct single_list_head father;
}node_type_name;

typedef struct leaf_accept
{
    const char *brief_info;
    node_type_name page_type;
    show_dir_page page_deal;
}menu_leaf_accept;

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

void simulate_show_leaf_page(menu_leaf *leaf)
{
    printf("================\n");
    printf("[   %s  ]\n",leaf->brief_info);
    printf("================\n");
}

void simulate_show_list_page(menu_leaf_accept *menu)//由非叶子节点调用，显示子页面内容
{
    struct single_list_head *list_node = &menu->page_type.start;
    node_type_name *temp;
    menu_leaf *leaf_ui;
    menu_leaf_accept *contain_ui;
    unsigned char cnt = 0;
    printf("========%s========\n",menu->brief_info);
    single_list_for_each_entry(temp,list_node,hook)
    {
        if(temp->type == LEAF){
            leaf_ui = list_entry(temp,menu_leaf,page_type);
            if(cnt == cur_mode.cur_choose)
            printf("==>:%s\n",leaf_ui->brief_info);
            else
            printf("    %s\n",leaf_ui->brief_info);
        }else if(temp->type == NON_LEAF){
            contain_ui = list_entry(temp,menu_leaf_accept,page_type);
            if(cnt == cur_mode.cur_choose)
            printf("==>:%s\n",contain_ui->brief_info);
            else
            printf("    %s\n",contain_ui->brief_info);
        }
        cnt++;
    }
    printf("================\n");
}


//建树
//通过指定父亲和儿子来构件关系
void tree_node_binding_by_ps(menu_leaf_accept *accept, menu_leaf *leaf)
{
    single_list_add_tail(&leaf->page_type.hook, &accept->page_type.start);
    leaf->page_type.father.next = &accept->page_type.start;
}

void tree_node_binding_by_ps_accept(menu_leaf_accept *accept,menu_leaf_accept *leaf)
{
    single_list_add_tail(&leaf->page_type.hook, &accept->page_type.start);
    leaf->page_type.father.next = &accept->page_type.start;
}

void set_accept_brief_info(menu_leaf_accept *accept,const char *text)
{
    accept->page_type.hook.next = NULL;
    accept->page_type.father.next = NULL;
    accept->page_type.start.next = NULL;
    accept->page_type.type = NON_LEAF;
    accept->page_deal = simulate_show_list_page;
    accept->brief_info = text;
}
void set_leaf_brief_info(menu_leaf *leaf, const char *text)
{
    leaf->page_type.hook.next = NULL;
    leaf->page_type.father.next = NULL;
    leaf->page_type.start.next = NULL;
    leaf->page_type.type =  LEAF;
    leaf->page_deal = simulate_show_leaf_page;
    leaf->brief_info = text;
}



unsigned char get_menu_choose_cnt(struct single_list_head *ptr)
{
    unsigned char cnt=0;
    node_type_name *pos;
    single_list_for_each_entry(pos,ptr,hook)
    {
        cnt++;
    }
    return cnt;
}

void get_uplist_from_curlisthead(struct cur_indicate *curmode)
{
    node_type_name *pos;
    menu_leaf_accept *contain_ui;
    menu_leaf *leaf_ui;
    struct single_list_head *ptr = curmode->cur_list_head;
    pos = list_entry(ptr,node_type_name,start);
    if(pos->type == LEAF){
        leaf_ui = list_entry(pos,menu_leaf,page_type);
        if(leaf_ui->page_type.father.next)
        curmode->cur_list_head = leaf_ui->page_type.father.next;
    }else{
        contain_ui = list_entry(pos,menu_leaf_accept,page_type);
        if(contain_ui->page_type.father.next)
        curmode->cur_list_head = contain_ui->page_type.father.next;
    }
}




void show_change_choose(void)
{
    menu_leaf *leaf_ui;
    menu_leaf_accept *contain_ui;
    node_type_name *pos;
    unsigned char cnt = 0;
    struct single_list_head *ptr = cur_mode.cur_list_head;
    pos = list_entry(ptr,node_type_name,start);
    if(pos->type == LEAF){
        leaf_ui = list_entry(pos,menu_leaf,page_type);
        leaf_ui->page_deal(leaf_ui);
    }else if(pos->type == NON_LEAF){
        contain_ui = list_entry(pos,menu_leaf_accept,page_type);
        contain_ui->page_deal(contain_ui);
    }
}

void show_curmode_ui(void)
{
    node_type_name *pos;
    menu_leaf *leaf_ui;
    menu_leaf_accept *contain_ui;
    struct single_list_head *ptr = cur_mode.cur_list_head;

    pos = list_entry(ptr,node_type_name,start);
    if(pos->type == LEAF){
        leaf_ui = list_entry(pos,menu_leaf,page_type);
        leaf_ui->page_deal(leaf_ui);
    }else if(pos->type == NON_LEAF){
        contain_ui = list_entry(pos,menu_leaf_accept,page_type);
        contain_ui->page_deal(contain_ui);
    }
}


void enter_return_new_page(struct cur_indicate *cur, unsigned char mode)
{
    node_type_name *pos;
    unsigned char cnt = 0;
    menu_leaf *leaf_ui;
    menu_leaf_accept *contain_ui;
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
                    cur_mode.cur_list_head = &leaf_ui->page_type.start;//重新初始list
                }else if(pos->type == NON_LEAF){
                    contain_ui = list_entry(pos,menu_leaf_accept,page_type);
                    contain_ui->page_deal(contain_ui);
                    cur_mode.cur_type = NON_LEAF;
                    cur_mode.cur_list_head = &contain_ui->page_type.start;
                }
                cur->chosse_cnt = get_menu_choose_cnt(cur_mode.cur_list_head);//注意摆放的位置
                break;
            }
            cnt++;
        }
    }else{//返回上一级
    
        get_uplist_from_curlisthead(&cur_mode);
        cur_mode.cur_type = NON_LEAF;
        cur_mode.cur_choose = 0;//注意摆放的位置
        show_curmode_ui();
        cur->chosse_cnt = get_menu_choose_cnt(cur_mode.cur_list_head);//注意摆放的位置
        
    }
}
void main()
{
    menu_leaf_accept root;
    menu_leaf_accept first_level_one;
    menu_leaf first_level_two;
    menu_leaf second_level_one;
    menu_leaf_accept second_level_two;
    menu_leaf third_level_one;
    unsigned char cmd;
    

    

    set_accept_brief_info(&root,"menu root");
    
    set_accept_brief_info(&first_level_one,"first_level_one");
    set_accept_brief_info(&second_level_two,"second_level_two");

    set_leaf_brief_info(&first_level_two,"01");
    set_leaf_brief_info(&second_level_one,"000");
    set_leaf_brief_info(&third_level_one,"0011");

    tree_node_binding_by_ps_accept(&root,&first_level_one);
    tree_node_binding_by_ps(&root,&first_level_two);

    tree_node_binding_by_ps(&first_level_one,&second_level_one);
    tree_node_binding_by_ps_accept(&first_level_one,&second_level_two);

    tree_node_binding_by_ps(&second_level_two,&third_level_one);

    cur_mode.cur_list_head = &root.page_type.start;
    cur_mode.cur_type = NON_LEAF;
    cur_mode.cur_choose = 0;
    cur_mode.chosse_cnt = get_menu_choose_cnt(cur_mode.cur_list_head);
    printf("test segmentation:%d\n",cur_mode.chosse_cnt);

    show_curmode_ui();
    while(1)
    {
        // if(kbhit())//<conio.h>
        // {
            cmd = getchar();
            system("stty echo");
            switch (cmd)
            {
            case 'w'://光标向上
                cur_mode.cur_choose--;
                if(cur_mode.cur_choose<0)
                {
                    cur_mode.cur_choose = cur_mode.chosse_cnt - 1;
                }
                show_change_choose();
                break;
            case 'a'://返回
                enter_return_new_page(&cur_mode,RETURN_PAGE);
                break;
            case 's'://光标向下
                cur_mode.cur_choose = (++cur_mode.cur_choose)%cur_mode.chosse_cnt;
                show_change_choose();
                break;
            case 'd'://进入
                enter_return_new_page(&cur_mode,ENTER_PAGE);
                break;
            default:
                break;
            }
            cmd = 'l';
        // }


    }



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

