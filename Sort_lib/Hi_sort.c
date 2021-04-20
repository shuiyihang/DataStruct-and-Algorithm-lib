#include "stdlib.h"
#include "stdio.h"
//通用的排序算法
//链表里的一个参数，把链表顺序换掉


/**
 * @brief 常用排序算法汇总,当然尽量推荐使用快排了
 * 分别是:
 *          冒泡
 *          选择
 *          插入
 *          快排
 * @param data      数据
 * @param len       数据长度
*/


void bubble_sort(int data[], int len)
{
    for(int i = 0; i<len; i++){
        for(int j=0; j<len-i-1; j++){
            if(data[j]>data[j+1]){
                int temp = data[j];
                data[j] = data[j+1];
                data[j+1] = temp;
            }
        }
    }
}


void select_sort(int data[], int len)
{
    for(int i = 0;i<len; i++){
        int min=i;
        for(int j = i+1;j<len; j++){
            if(data[min]>data[j]){
                min=j;
            }
        }
        if(min != i){
            int temp = data[i];
            data[i] = data[min];
            data[min] = temp;
        }
    }
}

void insert_sort(int data[], int len)
{
    for(int i = 1; i < len; i++){
        int ok_top = i-1;
        int current = data[i];
        while(current < data[ok_top]&&ok_top >= 0){
            data[ok_top+1] = data[ok_top];
            ok_top--;
        }
        data[ok_top+1] = current;
    }
}





static int _quick_sort_part(int data[],int start, int end)
{
    int divide = data[start];
    int prev_cur = start , tail_cur = end;
    while(prev_cur < tail_cur){
        while(prev_cur < tail_cur && divide <= data[tail_cur] ){
            tail_cur--;
        }
        if(prev_cur < tail_cur){
            data[prev_cur++] = data[tail_cur];
        }
        while (prev_cur < tail_cur && divide > data[prev_cur])
        {
            prev_cur++;
        }
        if(prev_cur < tail_cur){
            data[tail_cur--] = data[prev_cur];
        }
        
    }
    data[prev_cur] = divide;
    return prev_cur;
}


void quick_sort(int data[], int start, int end)
{
    if(start > end){
        return;
    }
    int divide = _quick_sort_part(data, start, end);
    quick_sort(data,start,divide-1);
    quick_sort(data,divide+1,end);
}