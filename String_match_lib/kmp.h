#ifndef __KMP_H__
#define __KMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define     u8_t        unsigned char
#define     u16_t       unsigned int

int kmp(u8_t *pat_str, int pat_str_len, u8_t *tar_str, int tar_str_len);


#ifdef __cplusplus
}
#endif

#endif