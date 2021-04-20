
#include "kmp.h"
#include "stdio.h"
static void __get_next_info(u8_t *pat_str,int *next,int pat_str_len)
{
    int cur=0;
    int index=-1;
    next[0]=-1;
    while (cur<pat_str_len-1)
    {
        if(index==-1 || pat_str[cur]==pat_str[index]){
            next[++cur] = ++index;
        }else{
            index = next[index];
        }
    }
    
}

/**
 * @brief 在主串里查找目标串
 *        用法
 *            1. example:接受数据的缓冲里查找特定的起始头
 * @param pat_str       主串
 * @param pat_str_len   主串长度
 * @param tar_str       目标串
 * @param tar_str_len   目标串长度  
 * 
 * @return 成功返回目标串位置,失败返回-1
 * 
*/

int kmp(u8_t *pat_str, int pat_str_len, u8_t *tar_str, int tar_str_len)
{
    int next[pat_str_len];

    int pat_cur=0;
    int tar_index=0;
    __get_next_info(pat_str,next,pat_str_len);

    #ifdef VIEW_NEXT
    for(int i=0;i<pat_str_len;i++){
        printf("%d ",next[i]);
    }
    printf("\n");
    #endif

    while (pat_cur<pat_str_len&&tar_index<tar_str_len)
    {
        if(tar_index==-1 || pat_str[pat_cur]==tar_str[tar_index]){
            pat_cur++;
            tar_index++;
        }else{
            tar_index = next[tar_index];
        }
    }
    if(tar_index==tar_str_len){
        return pat_cur-tar_str_len;
    }else{
        return -1;
    }
    


}