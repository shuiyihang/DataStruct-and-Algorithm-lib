
#include "data_types.h"
#include "string.h"
#include "lx_msg_deal.h"
#include "board.h"
#include "power_manager.h"

#include "app_defines.h"
#include "app_message.h"
#include "lx_app_api.h"

#include "audio_effect.h"
#include "sys_manager.h"

#include "fw_version.h"
#include "lx_define.h"

#include <adi_cmd.h>
#include "hotplug.h"

#include "lx_led_ctrl.h"
#include "adi_cmd.h"

#include "app_ui.h"

#include "lx_include/lx_nvram_config.h"
#ifdef CONFIG_LX_RGB_LED_DEV
#include <lx_rgbled_api.h>
extern void csb_send_data_light(csb_sync_info_steady_t *p);
#endif
// extern os_delayed_work app_cmd_disc_delaywork;

const u8_t reply_head[] =   {0x09, 0xFF, 0x00, 0x00, 0x01};
void anker_re_cmd_rgb_led_type(void);

/******************************************/

static inline u8_t cs_creat(u8_t *buf,u16_t len)
{
    int sum = 0;
    for(int i = 0;i<len;i++){
        sum += buf[i];
    }
    return sum;
}

inline u8_t __pow(u8_t base, u8_t index)
{
    u8_t num = 1;
    for(int i = 0; i < index; i++){
        num *= base;
    }
    return num;
}

static void lx_pack_and_send_msg(u8_t *payload_buf,u16_t buf_len,u8_t cmd_group,u8_t cmd)
{
    /*打包数据*/
    u8_t send_buf[sizeof(reply_head)+buf_len+5];//5 :   cmd_group cmd hh ll cs

    u8_t head_len = sizeof(reply_head);
    //回复的包头
    memcpy(send_buf,reply_head,head_len);

    send_buf[head_len] = cmd_group;
    send_buf[head_len+1] = cmd;

    send_buf[head_len+2] = sizeof(send_buf)&0xff;
    send_buf[head_len+3] = (sizeof(send_buf)>>8)&0xff;

    if(payload_buf){
        memcpy(&send_buf[head_len+4],payload_buf,buf_len);
    }

    send_buf[head_len+4+buf_len] = cs_creat(send_buf,head_len+buf_len+4);

    /*数据放入缓冲*/
    extern void lx_put_pkg_to_ringbuf(u8_t *pkg, u16_t len);

    lx_put_pkg_to_ringbuf(send_buf, sizeof(send_buf));

}


u8_t __choose_cmd_group(u8_t index, const lx_cmd_msg_list_t** cmd_list, u16_t *high);

void lx_app_msg_deal(u8_t *buf, u16_t len)
{
    //查找命令对应事件
    u16_t low = 0, high = 0, mid = 0;

    const lx_cmd_msg_list_t* lx_cmd_msg_list;

    if( __choose_cmd_group(buf[0], &lx_cmd_msg_list, &high) ){
        SYS_LOG_INF("error group:%#x\n",buf[0]);
        return;
    }
    while(low <= high){
        mid = (low + high)/2;
        if(buf[1] < lx_cmd_msg_list[mid].cmd){
            high = mid - 1;
        }else if(buf[1] > lx_cmd_msg_list[mid].cmd){
            low = mid + 1;
        }else{
            SYS_LOG_INF("%s\n",lx_cmd_msg_list[mid].info);
            lx_cmd_msg_list[mid].lx_msg_deal(&buf[4],len-4,buf[0]);
            break;
        }
    }
}

extern int hotplug_get_usbhost_state(void);
extern int hotplug_get_linein_state(void);
extern int bt_manager_get_phone_dev_num(void);

inline void __get_sound_src(u8_t temp[4])
{
    LINE_SOURCE_TYPE_E src = adi_cmd_get_cur_source();

    if(src == ADI_SOURCE_BT){
        temp[0] = 2;
    }else if(src == ADI_SOURCE_LINEIN){
        temp[0] = 1;
    }else if(src == ADI_SOURCE_USB){
        temp[0] = 0;
    }else{
        temp[0] = 3;// daisy
    }

    if(is_sound_src_exist(USB_EXIST_BIT)){
        temp[1] = 1;
    }else{
        temp[1] = 0;//未就绪
    }

    if(is_sound_src_exist(LINEIN_EXIST_BIT)){
        temp[2] = 1;
    }else{
        temp[2] = 0;//未就绪
    }

    if(bt_manager_get_phone_dev_num()){
        temp[3] = 1;
    }else{
        temp[3] = 0;
    }

}

static int __get_media_stream_type()
{
    int ret = AUDIO_STREAM_MUSIC;
    LINE_SOURCE_TYPE_E src = adi_cmd_get_cur_source();
    switch (src)
    {
    case ADI_SOURCE_BT:
        ret = AUDIO_STREAM_MUSIC;
        break;
    case ADI_SOURCE_LINEIN:
        ret = AUDIO_STREAM_LINEIN;
        break;
    case ADI_SOURCE_USB:
        ret = AUDIO_STREAM_LOCAL_MUSIC;
        break;
    case ADI_DAISY:
        ret = AUDIO_STREAM_DAISY_IN;
        break;
    default:
        break;
    }
    return ret;
}


/********************命令处理**********************/

extern uint32_t bt_manager_local_dev_role(void);
extern void get_adi_info_val(exten_get_val_t* val);

inline u8_t __bat_level(void)
{
    u8_t bat_level = get_system_battery_capacity();
    if (bat_level >= 81) {
		bat_level = 0x05;
	} else if (bat_level >= 61) {
		bat_level = 0x04;
	} else if (bat_level >= 41) {
		bat_level = 0x03;
	} else if (bat_level >= 21) {
		bat_level = 0x02;
	} else if (bat_level >= 6){
		bat_level = 0x01;
	}else{
        bat_level = 0x00;
    }
    return bat_level;
}
void lx_msg_fetch_devinfo(u8_t *buf, u16_t len, u8_t group)
{
    lx_dev_info_t dev_info;
	rgb_led_nvram_t led_nvram;
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_get_led_nvram(rgb_dev,&led_nvram);

    memset(&dev_info,0xFF,sizeof(lx_dev_info_t));
    dev_info.volume = sys_manager_get_volume(AUDIO_STREAM_MUSIC);
    dev_info.bat_level = __bat_level();
    dev_info.is_charge = __get_real_charge_status();

    if(bt_manager_local_dev_role() > 2){
        dev_info.csb_state = 1;//csb master/salve mode
    }else{
        dev_info.csb_state = 0;
    }
    //bassup
    dev_info.bass_state = get_bass_status();

    __get_sound_src((u8_t*)&dev_info.sound_src);

    //提示音
    dev_info.tts_state = get_tts_state();

    //自动开关机
    dev_info.auto_power.enable = is_autoPowerOff();
    dev_info.auto_power.time = get_autoPowerOff_time();


    const struct fw_version *ver = fw_version_get_current();//0x00036

    dev_info.fw_info.fw[0] = ((ver->version_code >> 8 )& 0xFF) / 10 + '0';
    dev_info.fw_info.fw[1] = ((ver->version_code >> 8 )& 0xFF) % 10 + '0';
    dev_info.fw_info.fw[2] = '.';
    dev_info.fw_info.fw[3] = (ver->version_code & 0xFF) / 10 + '0';
    dev_info.fw_info.fw[4] = (ver->version_code & 0xFF) % 10 + '0';
    
    //SN 16bytes

    // get_nvram_sn(dev_info.sn_code);//
    memcpy(dev_info.sn_code,get_user_sn(),16);

   	{
    	if(led_nvram.led_type == LED_TYPE_OFF)
    	{
    		dev_info.light_mod.onoff = 0;
    		dev_info.light_mod.mode = led_nvram.led_type_last - 1;
    		dev_info.light_mod.color = led_nvram.color_mode.color_mode[led_nvram.led_type_last - LED_TYPE_MODE_COLOR_START];
    	}
    	else if(led_nvram.led_type >= LED_TYPE_MODE_SPECIAL_START)
    	{
    		dev_info.light_mod.onoff = 1;
    		dev_info.light_mod.mode = led_nvram.led_type_last - 1;
    		dev_info.light_mod.color = led_nvram.color_mode.color_mode[led_nvram.led_type_last - LED_TYPE_MODE_COLOR_START];
    	}
        else
        {
    		dev_info.light_mod.onoff = 1;
    		dev_info.light_mod.mode = led_nvram.led_type - 1;
    		dev_info.light_mod.color = led_nvram.color_mode.color_mode[led_nvram.led_type - LED_TYPE_MODE_COLOR_START];
        }
	}

    //eq 2bytes + 10 custom
    u16_t eq_index = get_eq_index();
    dev_info.eq_mod.index[0] = (u8_t)eq_index;
    dev_info.eq_mod.index[1] = eq_index >> 8;


    uint8 *custom_eq = get_custom_eq();
    memcpy(dev_info.eq_mod.custom_eq, custom_eq, 9);
    //dj
    _rgb_led_get_belt_light_lndex(rgb_dev,&dev_info.dj_mod.index);
    _rgb_led_get_belt_light_color(rgb_dev,(u8_t*)dev_info.dj_mod.rgb);

    //game
    _rgb_led_get_game(rgb_dev,&dev_info.game_mod);

    //extension
    get_adi_info_val(&dev_info.exten_set);

    lx_pack_and_send_msg((u8_t*)&dev_info,sizeof(lx_dev_info_t),group,msg_devInfo);

}
void lx_msg_fetch_battery(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp[1];
    temp[0] = __bat_level();
    lx_pack_and_send_msg(temp,1,group,msg_bat_level);
}

void lx_msg_fetch_charge_state(u8_t *buf, u16_t len, u8_t group)//需要主动上报
{
    u8_t temp[1];
    temp[0] = user_get_charge_status();
    lx_pack_and_send_msg(temp,1,group,msg_charge_state);
}


void lx_msg_switch_eq(u8_t *buf, u16_t len, u8_t group)
{
    u16_t eq_index = buf[1] << 8 | buf[0];

    user_set_eq_index(eq_index,&buf[2]);

    lx_pack_and_send_msg(NULL, 0, group, msg_switch_eq);
}

void lx_msg_fetch_bassup_state(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp;
    //获取bass状态
    temp = get_bass_status();
    lx_pack_and_send_msg(&temp,1,group,msg_bassup_state);
}

// 获取设备音量
void lx_msg_fetch_volume(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp[1] = {0x00};
    temp[0] = sys_manager_get_volume(__get_media_stream_type());
    lx_pack_and_send_msg(temp,1,group,msg_vol_report);
}

// 获取播放暂停状态
extern int bt_manager_get_a2dp_dev_is_playing(void *dev_ptr);
void lx_msg_fetch_play_pause_state(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp[1] = {0x00};
    if(bt_manager_get_a2dp_dev_is_playing(NULL)){
        temp[0] = 1;
    }
    lx_pack_and_send_msg(temp,1,group,msg_play_pause_report);
}

void lx_msg_set_volume(u8_t *buf, u16_t len, u8_t group)
{
    u32_t set_ret;
    int media_type;
    sys_notify_event_filter_clear();
    media_type = __get_media_stream_type();
    set_ret = system_media_player_set_volume(media_type, buf[0]);
	lx_app_set_volume(media_type, buf[0]);

    if (set_ret == MAX_VOLUME_VALUE){
        sys_notify_status_send(SYS_EVENT_MAX_VOLUME);
    }
    else if (set_ret == MIN_VOLUME_VALUE){
        sys_notify_status_send(SYS_EVENT_MIN_VOLUME);
    }

    lx_pack_and_send_msg(NULL,0,group,msg_set_volume);
}


void lx_msg_play_status_ctl(u8_t *buf, u16_t len, u8_t group)
{
    struct app_msg msg = {0};
	msg.type = MSG_KEY_INPUT;

    msg.value = 0x4000005;
    lx_pack_and_send_msg(NULL,0,group,msg_set_play_pause);
    send_async_msg(APP_ID_MAIN, &msg);
}


void lx_msg_sound_src_rep(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp[4];
    
    __get_sound_src(temp);

    lx_pack_and_send_msg(temp,4,group,msg_sound_src_report);
}

void lx_msg_eq_report(u8_t *buf, u16_t len, u8_t group)
{
    uint8 *custom_eq = get_custom_eq();
    lx_pack_and_send_msg(custom_eq,9,group,msg_eq_report);
}

extern void tws_master_set_bass(u8_t bass);
void lx_msg_set_bassup_state(u8_t *buf, u16_t len, u8_t group)
{
    //设置bass状态
    set_bassup_status(buf[0]);
    lx_led_bass_up(buf[0]);
    set_adi_bass_mode(buf[0]);
    tws_master_set_bass(buf[0]);
    
    lx_pack_and_send_msg(NULL,0,group,msg_set_bassup);
}
void lx_msg_poweroff(u8_t *buf, u16_t len, u8_t group)
{
    struct app_msg msg = {0};
	msg.type = MSG_KEY_INPUT;
	msg.value = 0x80000002;  // 0x80000000 + 0x02  长按 + 键值
    send_async_msg(APP_ID_MAIN, &msg);
}

void lx_msg_fetch_exten_setting(u8_t *buf, u16_t len, u8_t group)
{
    exten_get_val_t temp_val;
    get_adi_info_val(&temp_val);

    lx_pack_and_send_msg((u8_t*)&temp_val,sizeof(exten_get_val_t),group,msg_get_exten_setting);
}

void lx_msg_csb_state_report(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp;
    if(bt_manager_local_dev_role() > 2){
        temp = 1;//csb master/salve mode
    }else{
        temp = 0;
    }
    lx_pack_and_send_msg(&temp,1,group,msg_csb_state_report);
}

extern void user_server_data_upload(void);
void lx_msg_get_upload_data(u8_t *buf, u16_t len, u8_t group)
{
    user_server_data_upload();
    user_server_data_t* temp = get_user_sevData_handle();
    lx_pack_and_send_msg((u8_t *)temp,sizeof(user_server_data_t),group,msg_get_sevData);
}

void lx_msg_cls_upload_data(u8_t *buf, u16_t len, u8_t group)
{
    clean_user_server_data();
    lx_pack_and_send_msg(NULL,0,group,msg_cls_sevData);
}

void lx_msg_set_log_upload(u8_t *buf, u16_t len, u8_t group)
{

    lx_pack_and_send_msg(NULL,0,group,msg_log_ctl);
}

void lx_msg_get_leftbelt_color(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp[15];
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_get_belt_light_color(rgb_dev,temp);

    lx_pack_and_send_msg(temp,15,group,msg_get_leftbelt_color);
}
 
void lx_msg_setdj_mode(u8_t *buf, u16_t len, u8_t group)
{
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_set_special_onoff(rgb_dev,buf[0],LED_TYPE_DJ_MODE);

    lx_pack_and_send_msg(NULL,0,group,msg_set_dj_mode);
}

void lx_msg_shortup_light(u8_t *buf, u16_t len, u8_t group)
{
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_set_belt_light_short_up(rgb_dev,buf);

    lx_pack_and_send_msg(NULL,0,group,msg_beltlight_short);
}

void lx_msg_set_leftbelt_color(u8_t *buf, u16_t len, u8_t group)
{
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_set_belt_light_color(rgb_dev,buf);

    lx_pack_and_send_msg(NULL,0,group,msg_set_leftbelt_color);
}

void lx_msg_set_beltlight_x_onoff(u8_t *buf, u16_t len, u8_t group)
{
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_set_belt_light_lndex(rgb_dev,buf);

    lx_pack_and_send_msg(NULL,0,group,msg_set_beltlight_x_onoff);
}

void lx_msg_longpress_light(u8_t *buf, u16_t len, u8_t group)
{
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_set_belt_light_long_up(rgb_dev,buf);

    lx_pack_and_send_msg(NULL,0,group,msg_beltlight_long);
}

void lx_msg_set_lightEffect(u8_t *buf, u16_t len, u8_t group)
{
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_set_game(rgb_dev,(rgb_led_game_struct_t *)buf);

    lx_pack_and_send_msg(NULL,0,group,msg_game_set_light);
}

void lx_msg_enter_exit_game(u8_t *buf, u16_t len, u8_t group)
{
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_set_special_onoff(rgb_dev,buf[0],LED_TYPE_GAME_MODE);

    lx_pack_and_send_msg(NULL,0,group,msg_game_onoff);
}

void lx_msg_get_light_effect_info(u8_t *buf, u16_t len, u8_t group)
{
    u8_t temp[2+LED_TYPE_MODE_COLOR_LEN];
	rgb_led_nvram_t led_nvram;
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_get_led_nvram(rgb_dev,&led_nvram);

    if(led_nvram.led_type == LED_TYPE_OFF)
    {
    	temp[0] = 0;
    	temp[1] = led_nvram.led_type_last - 1;
    }
    else if(led_nvram.led_type >= LED_TYPE_MODE_SPECIAL_START)
    {
    	temp[0] = 1;
    	temp[1] = led_nvram.led_type_last - 1;
    }
    else
    {
    	temp[0] = 1;
    	temp[1] = led_nvram.led_type - 1;
    }
    memcpy(temp+2,led_nvram.color_mode.color_mode,LED_TYPE_MODE_COLOR_LEN);

    lx_pack_and_send_msg(temp,2+LED_TYPE_MODE_COLOR_LEN,group,msg_get_light_effect_info);
}

void lx_msg_light_effect_report(u8_t *buf, u16_t len, u8_t group)
{
    anker_re_cmd_rgb_led_type();
}

void lx_msg_set_light_effect_info(u8_t *buf, u16_t len, u8_t group)
{
	csb_sync_info_steady_t p;
    if(buf[1] >= LED_TYPE_MODE_COLOR_LEN)
    {
        buf[1] = LED_TYPE_MODE_DEFAULT - 1;
    }
    if(buf[0])
    {
    	p.rgb_type = (buf[1]%LED_TYPE_MODE_LEN)+1;
        p.public.color = buf[2];
    	p.pack_type = PACK_MODE_BIT | PACK_COLOR_BIT;
    }
    else
    {
    	p.rgb_type = LED_TYPE_OFF;
        p.public.color = buf[2];
    	p.pack_type = PACK_MODE_BIT | PACK_COLOR_BIT;
    }
	csb_send_data_light(&p);

    lx_pack_and_send_msg(NULL,0,group,msg_set_light_effect_info);
}



void lx_msg_set_tone(u8_t *buf, u16_t len, u8_t group)
{
    set_tts_state(buf[0]);
    lx_pack_and_send_msg(NULL,0,group,msg_set_tone_onoff);
    lx_save_tts_state(buf[0]);
}

void lx_msg_set_auto_poweroff(u8_t *buf, u16_t len, u8_t group)
{
    set_autoPowerOff(buf[0]<<7 | buf[1]);
    set_auto_shutdown_reclock(1);// 设置之后重新计时关机时间
    lx_set_auto_power_save_time(5);//保存设置
    lx_pack_and_send_msg(NULL,0,group,msg_set_auto_poweroff);
}

void lx_msg_set_sound_source(u8_t *buf, u16_t len, u8_t group)
{
    lx_pack_and_send_msg(NULL,0,group,msg_set_sound_source);
    switch (buf[0])
    {
    case 0:
        lx_switch_sound_src(ADI_SOURCE_USB);
        break;
    case 1:
        lx_switch_sound_src(ADI_SOURCE_LINEIN);
        break;
    case 2:
        lx_switch_sound_src(ADI_SOURCE_BT);
        break;
    
    default:
        break;
    }
}

extern void set_adi_info_val(exten_set_val_t* val);
void lx_msg_set_exten_setting(u8_t *buf, u16_t len, u8_t group)
{
    set_adi_info_val((exten_set_val_t*)buf);
    lx_pack_and_send_msg(NULL,0,group,msg_set_exten_setting);
}

void lx_msg_disc_btble(u8_t *buf, u16_t len, u8_t group)
{
    lx_app_disconnect_all_phone();
    lx_pack_and_send_msg(NULL,0,group,msg_disc_dev);
    //启动一个工作队列，直到tx buf空，再断开ble
    // os_delayed_work_submit(&app_cmd_disc_delaywork, 30);
}

void lx_msg_set_csb_state(u8_t *buf, u16_t len, u8_t group)
{
    lx_pack_and_send_msg(NULL,0,group,msg_set_csb_state);

    if(bt_manager_local_dev_role() > 2){
        //csb master/salve mode
        if(buf[0] == 1){
            return;
        }
    }else{
        if(buf[0] == 0){
            return;
        }
    }

    struct app_msg msg = {0};
	msg.type = MSG_KEY_INPUT;
	msg.value = 0x0400002E;
    send_async_msg(APP_ID_MAIN, &msg);
}


/********************以下为小机主动上报状态的函数**********************/


void lx_bat_level_report(void)
{
    u8_t temp[1];
    temp[0] = __bat_level();
    lx_pack_and_send_msg(temp,1,GROUP_DEV_INFO,msg_bat_level);
}

void lx_charge_status_report(u8_t status)
{
    u8_t temp = status;
    lx_pack_and_send_msg(&temp,1,GROUP_DEV_INFO,msg_charge_state);
}

void lx_exten_setting_report()
{
    lx_msg_fetch_exten_setting(NULL,0,GROUP_DEV_INFO);
}

void lx_bassup_state_report(u8_t state)
{
    lx_pack_and_send_msg(&state,1,GROUP_EQ,msg_bassup_state);
}

void lx_sound_vol_report(void)
{
    u8_t temp[1] = {0x00};
    temp[0] = sys_manager_get_volume(__get_media_stream_type());
    lx_pack_and_send_msg(temp,1,GROUP_DEV_INFO,msg_vol_report);
}


void lx_play_status_report(u8_t mode)
{
    u8_t temp[1] = {0x00};
    temp[0] = mode;
    lx_pack_and_send_msg(temp,1,GROUP_DEV_INFO,msg_play_pause_report);
}

// 
void lx_sound_src_report(void)
{
    lx_msg_sound_src_rep(NULL,0,GROUP_DEV_INFO);
}

void lx_csb_state_report(void)
{
    lx_msg_csb_state_report(NULL,0,GROUP_DEV_INFO);
}


void anker_re_cmd_rgb_led_type(void)
{
    u8_t temp[3];
	rgb_led_nvram_t led_nvram;
	struct device* rgb_dev;

	rgb_dev = device_get_binding(CONFIG_RGB_LED_DEV_NAME);
	_rgb_led_get_led_nvram(rgb_dev,&led_nvram);

    if(led_nvram.led_type == LED_TYPE_OFF)
    {
    	temp[0] = 0;
    	temp[1] = led_nvram.led_type_last - 1;
    	temp[2] = led_nvram.color_mode.color_mode[led_nvram.led_type_last - LED_TYPE_MODE_COLOR_START];
    }
    else if(led_nvram.led_type >= LED_TYPE_MODE_SPECIAL_START)
    {
    	temp[0] = 1;
    	temp[1] = led_nvram.led_type_last - 1;
    	temp[2] = led_nvram.color_mode.color_mode[led_nvram.led_type_last - LED_TYPE_MODE_COLOR_START];
    }
    else
    {
    	temp[0] = 1;
    	temp[1] = led_nvram.led_type - 1;
    	temp[2] = led_nvram.color_mode.color_mode[led_nvram.led_type - LED_TYPE_MODE_COLOR_START];
    }

    lx_pack_and_send_msg(temp,3,GROUP_LIGHT,msg_light_effect_report);
}

/******************************************/
const lx_cmd_msg_list_t lx_cmd_msg_list_devinfo[] = {
    {GROUP_DEV_INFO,    msg_devInfo,            STRINGIFY(get_devinfo),                 lx_msg_fetch_devinfo},
    {GROUP_DEV_INFO,    msg_bat_level,          STRINGIFY(get_bat_level),               lx_msg_fetch_battery},
    {GROUP_DEV_INFO,    msg_charge_state,       STRINGIFY(get_charge_state),            lx_msg_fetch_charge_state},

    {GROUP_DEV_INFO,    msg_vol_report,         STRINGIFY(vol report),                  lx_msg_fetch_volume},
    {GROUP_DEV_INFO,    msg_sound_src_report,   STRINGIFY(sound src report),            lx_msg_sound_src_rep},
    {GROUP_DEV_INFO,    msg_play_pause_report,  STRINGIFY(get play/pause state),        lx_msg_fetch_play_pause_state},
    {GROUP_DEV_INFO,    msg_get_exten_setting,  STRINGIFY(get_exten_setting),           lx_msg_fetch_exten_setting},
    
    {GROUP_DEV_INFO,    msg_csb_state_report,  STRINGIFY(csb_state_report),             lx_msg_csb_state_report},

    {GROUP_DEV_INFO,    msg_set_auto_poweroff,  STRINGIFY(set_auto_poweroff),           lx_msg_set_auto_poweroff},
    {GROUP_DEV_INFO,    msg_set_volume,         STRINGIFY(set volume),                  lx_msg_set_volume},
    {GROUP_DEV_INFO,    msg_poweroff,           STRINGIFY(poweroff),                    lx_msg_poweroff},
    {GROUP_DEV_INFO,    msg_set_sound_source,   STRINGIFY(set_sound_source),            lx_msg_set_sound_source},
    {GROUP_DEV_INFO,    msg_set_tone_onoff,     STRINGIFY(set_tone),                    lx_msg_set_tone},
    {GROUP_DEV_INFO,    msg_set_play_pause,     STRINGIFY(set play/pause),              lx_msg_play_status_ctl},
    {GROUP_DEV_INFO,    msg_set_exten_setting,  STRINGIFY(set_exten_setting),           lx_msg_set_exten_setting},
    {GROUP_DEV_INFO,    msg_disc_dev,           STRINGIFY(disconnect_btdev),            lx_msg_disc_btble},

    {GROUP_DEV_INFO,    msg_set_csb_state,      STRINGIFY(set_csb_state),               lx_msg_set_csb_state},

};


const lx_cmd_msg_list_t lx_cmd_msg_list_eq[] = {
    {GROUP_EQ,    msg_eq_report,                  STRINGIFY(eq report),             lx_msg_eq_report},
    {GROUP_EQ,    msg_bassup_state,               STRINGIFY(get bassup state),      lx_msg_fetch_bassup_state},
    {GROUP_EQ,    msg_switch_eq,                  STRINGIFY(change eq),             lx_msg_switch_eq},
    {GROUP_EQ,    msg_set_bassup,                 STRINGIFY(set bassup state),      lx_msg_set_bassup_state},
};
const lx_cmd_msg_list_t lx_cmd_msg_list_sevdat[] = {
    {GROUP_SEV_DAT,    msg_get_sevData,           STRINGIFY(get upload data),         lx_msg_get_upload_data},
    {GROUP_SEV_DAT,    msg_cls_sevData,           STRINGIFY(cls upload data),         lx_msg_cls_upload_data},
    {GROUP_SEV_DAT,    msg_log_ctl,               STRINGIFY(sevData upload onoff),    lx_msg_set_log_upload},
    //这里有一条命令，主动上报的内容
};
const lx_cmd_msg_list_t lx_cmd_msg_list_dj[] = {
    {GROUP_DJ,    msg_get_leftbelt_color,         STRINGIFY(get leftbeltcolor),         lx_msg_get_leftbelt_color},
    {GROUP_DJ,    msg_set_dj_mode,                STRINGIFY(set dj mode),               lx_msg_setdj_mode},//0x81
    {GROUP_DJ,    msg_set_leftbelt_color,         STRINGIFY(set leftbeltcolor),         lx_msg_set_leftbelt_color},//0x83
    {GROUP_DJ,    msg_set_beltlight_x_onoff,      STRINGIFY(set beltlight_x_onoff),     lx_msg_set_beltlight_x_onoff},
    {GROUP_DJ,    msg_beltlight_short,            STRINGIFY(shortUp light),             lx_msg_shortup_light},//0x82
    {GROUP_DJ,    msg_beltlight_long,             STRINGIFY(longPress light),           lx_msg_longpress_light},
};
const lx_cmd_msg_list_t lx_cmd_msg_list_game[] = {
    {GROUP_GAME,    msg_game_set_light,           STRINGIFY(set game light),        lx_msg_set_lightEffect},
    {GROUP_GAME,    msg_game_onoff,               STRINGIFY(on/off Partygame),      lx_msg_enter_exit_game},
};

const lx_cmd_msg_list_t lx_cmd_msg_list_light[] = {
    {GROUP_LIGHT,    msg_get_light_effect_info,   STRINGIFY(get lightEffect info),       lx_msg_get_light_effect_info},
    {GROUP_LIGHT,    msg_light_effect_report,     STRINGIFY(light effect report),        lx_msg_light_effect_report},
    {GROUP_LIGHT,    msg_set_light_effect_info,   STRINGIFY(set light effect info),      lx_msg_set_light_effect_info},
};


const u16_t LIST_DEVINFO_CNT = (sizeof(lx_cmd_msg_list_devinfo)/sizeof(lx_cmd_msg_list_t));

const u16_t LIST_EQ_CNT = (sizeof(lx_cmd_msg_list_eq)/sizeof(lx_cmd_msg_list_t));
const u16_t LIST_SEVDAT_CNT = (sizeof(lx_cmd_msg_list_sevdat)/sizeof(lx_cmd_msg_list_t));
const u16_t LIST_DJ_CNT = (sizeof(lx_cmd_msg_list_dj)/sizeof(lx_cmd_msg_list_t));
const u16_t LIST_GAME_CNT = (sizeof(lx_cmd_msg_list_game)/sizeof(lx_cmd_msg_list_t));
const u16_t LIST_LIGHT_CNT = (sizeof(lx_cmd_msg_list_light)/sizeof(lx_cmd_msg_list_t));

u8_t __choose_cmd_group(u8_t index, const lx_cmd_msg_list_t** cmd_list, u16_t *high)
{
    u8_t res = 0;
    switch (index)
    {
    case GROUP_DEV_INFO:
        /* code */
        *cmd_list = lx_cmd_msg_list_devinfo;
        *high = LIST_DEVINFO_CNT - 1;
        break;
    case GROUP_EQ:
        *cmd_list = lx_cmd_msg_list_eq;
        *high = LIST_EQ_CNT - 1;
        break;
    case GROUP_SEV_DAT:
        *cmd_list = lx_cmd_msg_list_sevdat;
        *high = LIST_SEVDAT_CNT - 1;
        break;
    case GROUP_DJ:
        *cmd_list = lx_cmd_msg_list_dj;
        *high = LIST_DJ_CNT - 1;
        break;
    case GROUP_GAME:
        *cmd_list = lx_cmd_msg_list_game;
        *high = LIST_GAME_CNT - 1;
        break;
    case GROUP_LIGHT:
        *cmd_list = lx_cmd_msg_list_light;
        *high = LIST_LIGHT_CNT - 1;
        break;
    
    default:
        res = 1;
        break;
    }
    return res;
}
