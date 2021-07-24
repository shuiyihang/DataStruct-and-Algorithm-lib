/**
 * todo:
 * 根据常态和notify来决定静态图片刷完需不需要重复
 * 常态被打断，notify正常停止或者非正常停止，要恢复常态
 * 
 * 
 * 主从机同步灯效
*/

#include "os_common_api.h"
#include "light_rgb_core.h"
#include "device.h"
#include "mem_manager.h"
#include "light_rgb_api.h"
#include "string.h"

#define SPI_SEND_INTERVAL   30


rgb_group_manager_t rgb_group_data;


static bool led_rgb_enable = false;//默认禁用

extern const light_ctl_param_t light_crl_param_list[];
extern const light_view_config_t light_view_config_list[];
extern const static_gif_t light_gif_lib_list[];
extern const u32_t LIFGHT_VIEW_NUMS;

static bool rgb_color_updata_work(void)
{
    if(rgb_group_data.rgb_update_handle){
        rgb_group_data.rgb_update_handle();//暂时状态要自己结束
        return true;
    }
    return false;
}

static void rgb_spi_sendbuf_work(void)
{
    //填充数据,具体的颜色转为24bit的grb
    for(int index = 0; index < RGB_NUMS_IN_GROUP; index++){
        spi_buf_fill_24bit(rgb_group_data.spi_data_buf[index],RGB565toGRB888(rgb_group_data.color_buf[index].color_16));
    }
    //spi发送
    struct spi_buf tx_info;
    tx_info.buf = rgb_group_data.spi_data_buf;
    tx_info.len = RGB_NUMS_IN_GROUP * COLOR_FORMAT_BITS;
    spi_transceive(&rgb_group_data.spi_cfg, &tx_info, 1, NULL, 0);
}

void light_rgb_repeat_work(os_work *work)
{
    if(!led_rgb_enable)
        return;
    if(rgb_color_updata_work()){
        rgb_spi_sendbuf_work();
    }
    //提交下一次任务
    os_delayed_work_submit(&rgb_group_data.light_repeat_work,SPI_SEND_INTERVAL);
}

void rgb_dev_enable()
{
    led_rgb_enable = true;
    os_delayed_work_submit(&rgb_group_data.light_repeat_work,0);
}

void rgb_dev_disable()
{
    //所有灯关掉
    memset(rgb_group_data.color_buf, 0, RGB_NUMS_IN_GROUP * sizeof(rgb_color_t));
    rgb_spi_sendbuf_work();
    led_rgb_enable = false;
}

void rgb_dev_pause()
{
    led_rgb_enable = false;
}

int lx_rgb_led_init(struct device *dev)
{
    rgb_group_manager_t *rgb_group_ctx = dev->driver_data;

    /*spi init*/
    spi_dev_init(&rgb_group_ctx->spi_cfg);

    /*malloc buff*/
    rgb_group_ctx->color_buf = mem_malloc(RGB_NUMS_IN_GROUP * sizeof(rgb_color_t));
    __ASSERT(rgb_group_ctx->color_buf != NULL,"color_buf malloc failed\n");

    rgb_group_ctx->spi_data_buf = mem_malloc(RGB_NUMS_IN_GROUP * COLOR_FORMAT_BITS);
    __ASSERT(rgb_group_ctx->spi_data_buf != NULL, "spi_data_buf malloc failed\n");


    k_delayed_work_init(&rgb_group_ctx->light_repeat_work,light_rgb_repeat_work);

    /*init state*/
    rgb_group_data.rgb_update_handle = light_refresh_with_gif;
    rgb_group_data.steady_state = STEADY_STATE_NULL;

    SYS_LOG_INF("light rgb init ok\n");

    return 0;
}

//更换刷图刷新方式
void rgb_modify_update_handle(light_update_style_t style)
{
    switch (style)
    {
    case DEFAULT_STATIC_GIF:
        rgb_group_data.rgb_update_handle = light_refresh_with_gif;
        break;
    case DRV_TEST:
        rgb_group_data.rgb_update_handle = test_blue_style;
        break;
    default:
        break;
    }
}

//只是更新使用哪张gif,更换刷图的gif
void rgb_modify_gif_source(u32_t ui_event)//普通刷图，还是自己计算来刷
{
    const static_gif_t *image = NULL;
    gif_manager_t *gif_manage = &rgb_group_data.vary_src;
    u16_t i,index;
    //需要增加控制
    if(!led_rgb_enable)
        led_rgb_enable = true;

    for(i=0; i < LIFGHT_VIEW_NUMS; i++){
        if(light_view_config_list[i].ui_event == ui_event){
            image = &light_gif_lib_list[light_view_config_list[i].gif_label];
            break;
        }
    }

    if(!image){
        SYS_LOG_INF("gif viewmap not exit\n");
        return;
    }
    
    index = light_view_config_list[i].param_index;

    if(light_crl_param_list[index].repeat_times == RGB_NO_LIMIT){
        rgb_group_data.steady_state = ui_event;
    }

    gif_manage->frame = 0;
    gif_manage->gif = image;
    gif_manage->ctl_param.interval = light_crl_param_list[index].interval;
    gif_manage->ctl_param.repeat_times = light_crl_param_list[index].repeat_times;
    //重新提交更新任务
    os_delayed_work_submit(&rgb_group_data.light_repeat_work,0);
}


const light_rgb_api_t lx_rgb_api =
{
    .light_rgb_enable = rgb_dev_enable,
    .light_rgb_disable = rgb_dev_disable,
    .light_rgb_pause = rgb_dev_pause,
    .light_rgb_notify = rgb_modify_gif_source,
    .light_rgb_set_style = rgb_modify_update_handle,
};




DEVICE_AND_API_INIT(light_led, CONFIG_RGB_LED_DEV_NAME,
		    lx_rgb_led_init,
		    &rgb_group_data, NULL,
		    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    &lx_rgb_api);