#ifndef __HI_GRAPH_H__
#define __HI_GRAPH_H__
#define     u8_t        unsigned char
#define     u16_t       unsigned int

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


void A_star(u8_t maze[][10],struct win_size maze_size, struct local_pos start, struct local_pos end);
#endif