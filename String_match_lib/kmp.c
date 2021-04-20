
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