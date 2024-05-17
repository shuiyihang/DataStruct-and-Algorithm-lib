#ifndef __LX_MSG_DEAL_H__
#define __LX_MSG_DEAL_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "data_types.h"
#include "string.h"
#include "sys_log.h"

#include "lx_app_api.h"

#include "lx_rgbled_api.h"

#ifndef SYS_LOG_DOMAIN
	#define SYS_LOG_DOMAIN "lx_ble_app"
#endif
#ifndef SYS_LOG_LEVEL
    #define SYS_LOG_LEVEL SYS_LOG_LEVEL_INFO
#endif


#define LX_RX_BUF_SIZE			(512)
#define LX_TX_BUF_SIZE		    (128)

#define LX_RXTX_INTERVAL		(5)

#define LX_PACK_HEAD            (5)


#define PKG_MAX_LEN             (128)//一包数据最长长度

/**命令所在组**/
typedef enum{
    GROUP_DEV_INFO =        0x01,
    GROUP_EQ =              0x02,

    GROUP_SEV_DAT =         0x05,
    GROUP_DJ =              0x0C,
    GROUP_GAME =            0x0D,
    GROUP_LIGHT =           0x0E,
}cmd_group_t;





/**命令**/
typedef enum {
    msg_devInfo                                 = 0x01,
    msg_bat_level                               = 0x03,
    msg_charge_state                            = 0x04,
    msg_vol_report                              = 0x09,//主动上报
    msg_sound_src_report                        = 0x0A,//主动上报
    msg_play_pause_report                       = 0x21,//播放暂停状态主动上报
    
    msg_get_exten_setting                       = 0x22,//需要主动上报
    msg_csb_state_report                        = 0x24,//主动上报

    msg_set_auto_poweroff                       = 0x86,
    msg_set_volume                              = 0x88,
    msg_poweroff                                = 0x89,
    msg_set_sound_source                        = 0x8B,
    msg_set_tone_onoff                          = 0x90,
    msg_set_play_pause                          = 0x91,
    msg_set_exten_setting                       = 0x92,
    msg_disc_dev                                = 0x93,//主动断开设备

    msg_set_csb_state                           = 0xA4,
}cmd_dev_info_t;

typedef enum {
    msg_eq_report                               = 0x01,//eq变化主动上报
    msg_bassup_state                            = 0x04,//主动上报
    msg_switch_eq                               = 0x81,
    msg_set_bassup                              = 0x84,
}cmd_eq_t;

typedef enum {
    msg_get_sevData                             = 0x01,
    msg_log_data                                = 0x02,
    msg_cls_sevData                             = 0x81,
    msg_log_ctl                                 = 0x82,
}cmd_sev_data_t;

typedef enum {
    msg_get_leftbelt_color                      = 0x02,
    msg_set_dj_mode                             = 0x81,
    msg_set_leftbelt_color                      = 0x82,
    msg_set_beltlight_x_onoff                   = 0x83,
    msg_beltlight_short                         = 0x84,
    msg_beltlight_long                          = 0x85,
}cmd_dj_t;

typedef enum {
    msg_game_set_light                          = 0x81,
    msg_game_onoff                              = 0x82,
}cmd_party_game_t;

typedef enum {
    msg_get_light_effect_info                   = 0x01,
    msg_light_effect_report                     = 0x03,//灯效变化主动上报
    msg_set_light_effect_info                   = 0x83,
}cmd_light_effect_t;


typedef struct {
    cmd_group_t cmd_group;
    u8_t cmd;
    char info[32];
    void (*lx_msg_deal)(u8_t *buf, u16_t len, u8_t group);
}lx_cmd_msg_list_t;




typedef struct
{
    u8_t volume;
    u8_t bat_level;
    u8_t is_charge;
    u8_t csb_state;
    u8_t bass_state;
    struct {
        u8_t usb;
        u8_t linein;
        u8_t btmusic;
        u8_t daisy;
    }sound_src;

    u8_t tts_state;
    struct{
        u8_t enable;
        u8_t time;
    }auto_power;

    struct{
        u8_t fw[5];
    }fw_info;

    u8_t sn_code[16];
    struct {
        u8_t onoff;
        u8_t mode;
        u8_t color;
    }light_mod;
    struct {
        u8_t index[2];
        u8_t custom_eq[9];
    }eq_mod;

    struct {
        u8_t index;
        u8_t rgb[5][3];
    }dj_mod;// 63

    rgb_led_game_struct_t game_mod;

    exten_get_val_t exten_set;
    
}lx_dev_info_t;

extern void lx_app_set_volume(u32_t media_type, u32_t volume);


extern uint8_t __get_cur_app(void);
extern void lx_switch_sound_src(u8_t expect_type);

extern int get_nvram_sn(u8_t *sn_buff);
extern void set_eq_index(uint16_t val);

extern void set_custom_eq(uint8_t *buff);

extern void clean_user_server_data(void);

extern void lx_set_auto_power_save_time(uint8_t time);

extern void lx_app_disconnect_all_phone(void);

extern uint16_t get_eq_index(void);


extern uint8_t *get_custom_eq(void);

extern uint8_t get_bass_status(void);

extern void lx_save_tts_state(u8_t state);

#ifdef __cplusplus
}
#endif

#endif