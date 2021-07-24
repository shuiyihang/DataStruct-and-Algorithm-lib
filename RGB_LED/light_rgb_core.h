/**
 * TODO:
 * 整体结构的优化{工作队列位置，颜色值的获取}
 * 支持运算得到的图，而不只是刷图
 * 增加api,动态效果和静态效果
 * 
 * 用python 写一个可视化工具，来取指定gif逐帧颜色转
*/

#ifndef __LIGHT_RGB_CORE_H__
#define __LIGHT_RGB_CORE_H__

#include "spi.h"
#include "kernel.h"
#include "string.h"
#define SYS_LOG_DOMAIN "RGB_LED"
#define SYS_LOG_LEVEL SYS_LOG_LEVEL_DEBUG
#include <logging/sys_log.h>

#define RGB_NUMS_IN_GROUP   (4)//灯珠数量
#define COLOR_FORMAT_BITS   (24)

#define LED_DATA_1  0xfc
#define LED_DATA_0  0xc0


#define RGB_NO_LIMIT        (u16_t)(-1)
#define STEADY_STATE_NULL   (u8_t)(-1)

/*脚本生成的gif数据格式*/
typedef struct
{
    const u16_t *gif;
    u16_t frame_nums; 
}static_gif_t;

/*一些额外的控制参数*/
typedef struct
{
    u16_t repeat_times;
    u16_t interval;
}light_ctl_param_t;

/*事件对应 UI的gif和参数*/
typedef struct
{
    u8_t ui_event;
    u16_t gif_label;
    u16_t param_index;
}light_view_config_t;


//颜色统一使用rgb565格式,片上弃用32
typedef union
{
    struct rgb_led_pixel
    {
        u16_t blue:5;
        u16_t green:6;
        u16_t red:5;
    } color_565;
    u16_t color_16;
}rgb_color_t;


typedef struct
{
    const static_gif_t *gif;
    u16_t frame;
    light_ctl_param_t ctl_param;
}gif_manager_t;


typedef struct rgb_group_manager
{
    rgb_color_t *color_buf;
    void (*rgb_update_handle)(void);

    u8_t (*spi_data_buf)[24];

    u8_t steady_state;//稳态
    gif_manager_t vary_src;


    struct k_delayed_work light_repeat_work;

    struct spi_config  spi_cfg;
}rgb_group_manager_t;






extern rgb_group_manager_t rgb_group_data;



static inline void spi_buf_fill_24bit(u8_t *buf, u32_t grb_val)
{
    for(int i = 0; i < COLOR_FORMAT_BITS; i++){
        buf[COLOR_FORMAT_BITS-1-i] = ((grb_val >> i)&1)? LED_DATA_1: LED_DATA_0;
    }
}











void test_blue_style();
void light_refresh_with_gif(void);

u32_t RGB565toGRB888(u16_t grb);
void spi_dev_init(struct spi_config *cfg);

void rgb_modify_gif_source(u32_t ui_event);

void rgb_dev_enable();
void rgb_dev_disable();

#endif