/**
 * 风格
*/
#include "light_rgb_core.h"

static uint8_t cnt = 0;
void test_blue_style()
{
    u16_t color;
    cnt = (cnt+1)%15;
    if(cnt < 5 )
        color= 0x7e0;
    else if(cnt < 10)
        color = 0x3F;
    else
        color = 0xf800;
    for(int i = 0; i<RGB_NUMS_IN_GROUP; i++)
    {
        rgb_group_data.color_buf[i].color_16 = color;
    }
}



void light_refresh_with_gif()
{
    //根据当前gif和frame
    u16_t cur_frame = rgb_group_data.vary_src.frame;
    light_ctl_param_t *param = &rgb_group_data.vary_src.ctl_param;

    if(cur_frame != rgb_group_data.vary_src.gif->frame_nums){
        //FIXME
        memcpy(rgb_group_data.color_buf,&rgb_group_data.vary_src.gif->gif[cur_frame*RGB_NUMS_IN_GROUP],
        RGB_NUMS_IN_GROUP*sizeof(rgb_group_data.vary_src.gif->gif[0]));
        rgb_group_data.vary_src.frame++;
    }else{
        //是否需要重复
        if(param->repeat_times == RGB_NO_LIMIT){//稳态无条件重复
            rgb_group_data.vary_src.frame = 0;
        }else{
            if(param->repeat_times){//有限次重刷
                param->repeat_times--;
                rgb_group_data.vary_src.frame = 0;
            }else{//notify结束
                if(rgb_group_data.steady_state == STEADY_STATE_NULL){
                    rgb_dev_disable();
                }else{
                    rgb_modify_gif_source(rgb_group_data.steady_state);
                }
                
            }

        }
    }
}
