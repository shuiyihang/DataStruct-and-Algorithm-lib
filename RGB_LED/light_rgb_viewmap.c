#include "light_rgb_core.h"
#include "app_ui.h"
#include "light_rgb_giflib.h"


const light_ctl_param_t light_crl_param_list[] =
{
    {
        .repeat_times = 0,
        .interval = 0
    },
    {
        .repeat_times = 2,
        .interval = 0
    },
    {
        .repeat_times = RGB_NO_LIMIT,
        .interval = 0
    },
};

const static_gif_t light_gif_lib_list[] = 
{
    {LX_RGB_POWER_ON_GIF , 0x077},

};

/*      ui_event        gif_label           param_index*/
//lx_light_view_update(device_get_binding(CONFIG_RGB_LED_DEV_NAME),UI_EVENT_CONNECT_SUCCESS);
const light_view_config_t light_view_config_list[] = 
{
    {UI_EVENT_POWER_ON,             0 ,             0},
    {UI_EVENT_CONNECT_SUCCESS,      0 ,             1},
    {UI_EVENT_PLAY_START,           0 ,             2},
};


const u32_t LIFGHT_VIEW_NUMS = ARRAY_SIZE(light_view_config_list);
/**
 * 静态就是刷完从头开始
 * 动态就是有限次的刷gif
*/
