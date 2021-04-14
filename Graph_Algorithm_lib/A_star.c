
/**
 * 
*/
#include "stdlib.h"
#include "stdio.h"

#include "string.h"
#include "Hi_Graph.h"
#include "../Common_Struct_lib/Hi_PriorityQueue.h"
#include "../Common_Struct_lib/Hi_Stack.h"

#define     MAX_VALUE       65535
#define     OBSTACLE_VALUE  1

#define     u8_t        unsigned char
#define     u16_t       unsigned int

typedef struct 
{
    struct local_pos pos;
    struct priority_queue_info node_info;
}custom_data;


typedef struct
{
    struct local_pos pos;
    struct stack_head clue;
}stack_data;

static int __sub_abs(int a, int b)
{
    return a>b?a-b:b-a;
}

static custom_data* __get_content(struct list_head *node)
{
    prio_queue_info *temp=list_entry(node,prio_queue_info,prior_hook);
    custom_data* pop_data;

    pop_data = list_entry(temp,custom_data,node_info);
    list_del(&pop_data->node_info.prior_hook);
    return pop_data;
}
void A_star(u8_t maze[][10],struct win_size maze_size, struct local_pos start, struct local_pos end)
{
    int dis[maze_size.height][maze_size.width];
    struct local_pos trace[maze_size.height][maze_size.width];

    int dx[]={1,0,-1,0};
    int dy[]={0,1,0,-1};

    LIST_HEAD(queue);
    STACK_HEAD(stack);

    //数据初始化
    for(int i=0;i<maze_size.height;i++){
        for(int j=0;j<maze_size.width;j++){
            dis[i][j] = MAX_VALUE;
        }
    }


    custom_data *push_data = (custom_data*)malloc(sizeof(custom_data));
    custom_data *pop_data;
    push_data->pos = start;
    push_data->node_info.prior = 0;
    priority_queue_push(&push_data->node_info,&queue);
    dis[start.y][start.x] = 0;

    while(!list_empty(&queue)){
        unsigned char find = 0;
        pop_data=__get_content(queue.next);
        printf("%d===%d\n",pop_data->pos.x,pop_data->pos.y);

        for(int i=0;i<4;i++){
            struct local_pos smell;
            smell.x = pop_data->pos.x + dx[i];
            smell.y = pop_data->pos.y + dy[i];
            if((smell.x<maze_size.width && smell.x>=0)
             &&(smell.y<maze_size.height && smell.y>=0)
             &&(maze[smell.y][smell.x] != OBSTACLE_VALUE)){
                printf("\n");
                int temp_dis = dis[pop_data->pos.y][pop_data->pos.x]+1;
                if(temp_dis<dis[smell.y][smell.x]){
                    dis[smell.y][smell.x] = temp_dis;
                    trace[smell.y][smell.x] = pop_data->pos;

                    push_data = (custom_data*)malloc(sizeof(custom_data));
                    push_data->pos.x = smell.x;
                    push_data->pos.y = smell.y;
                    printf("%d+++%d\n",push_data->pos.x,push_data->pos.y);
                    push_data->node_info.prior = dis[smell.y][smell.x]+__sub_abs(smell.x,end.x)+__sub_abs(smell.y,end.y);//曼哈顿距离
                    priority_queue_push(&push_data->node_info,&queue);

                    if(smell.y == end.y && smell.x == end.x){
                        find=1;
                        break;
                    }



                }


             }
        }
        if(find){
            while (!list_empty(&queue))
            {

                pop_data=__get_content(queue.next);
                free(pop_data);
            }
            
            stack_data *cur_data=(stack_data*)malloc(sizeof(stack_data));
            cur_data->pos = end;
            stack_push(&cur_data->clue,&stack);
            while(cur_data->pos.x != start.x || cur_data->pos.y != start.y){
                stack_data *new_data=(stack_data*)malloc(sizeof(stack_data));
                new_data->pos = trace[cur_data->pos.y][cur_data->pos.x];
                stack_push(&new_data->clue,&stack);
                cur_data=new_data;
            }

            //TODO 根据栈中内容执行刷屏
            while(!stack_empty(&stack)){
                struct stack_head* temp_stack_node = stack_pop(&stack);
                stack_data *finally = list_entry(temp_stack_node,stack_data,clue);
                printf("<%d,%d>\n",finally->pos.y,finally->pos.x);
                free(finally);
            }
            return;
        }

        free(pop_data);

        
    }




}