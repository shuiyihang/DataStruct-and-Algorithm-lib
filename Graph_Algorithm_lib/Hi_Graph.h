#ifndef __HI_GRAPH_H__
#define __HI_GRAPH_H__

#ifdef __cplusplus
extern "C" {
#endif


#define     u8_t        unsigned char
#define     u16_t       unsigned int


#define     MAX_VALUE       65535
#define     OBSTACLE_VALUE  1

struct local_pos
{
    int x;
    int y;
};
struct win_size
{
    int width;
    int height;
};

static inline int __sub_abs(int a, int b)
{
    return a>b?a-b:b-a;
}


void A_star(u8_t maze[][10],struct win_size maze_size, struct local_pos start, struct local_pos end);


#ifdef __cplusplus
}
#endif

#endif