#include <string.h>
#include "encoder.h"

static const char *TAG = "encoder";

static encoder_t g_encoder;

static esp_timer_handle_t g_encoder_timer_handle;


const int8_t table[16] = {
0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 //自定义，一个栅格算一个
// 0, 0, -1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 0, 0 //自定义，一个栅格算两个
// 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0
};

const char* ev_to_text[] = {
    "Pressed",
    "LONG_Pressed",
    "Hold",

    "Released",
    "LONG_Press_released",

    "Clicked",
    "DoubleClicked",
    "None", 
};

static keybuff_t g_evt_buf;

static void timer_service(void *args)
{

#if ENC_DECODER == ENC_NORMAL
    int8_t curr = 0;
    if(gpio_get_level(EC11_A_PIN) == PIN_ACTIVE){
        curr = 3;
    }

    if(gpio_get_level(EC11_B_PIN) == PIN_ACTIVE){
        curr ^= 1;
    }

    int8_t diff = g_encoder.last - curr;
    if(diff & 1){
        g_encoder.last = curr;
        g_encoder.delta += (diff & 2) - 1;
    }
#else
    g_encoder.last = (g_encoder.last << 2) & 0x0F;

    if(gpio_get_level(EC11_A_PIN) == PIN_ACTIVE){
        g_encoder.last |= 2;
    }

    if(gpio_get_level(EC11_B_PIN) == PIN_ACTIVE){
        g_encoder.last |= 1;
    }

    g_encoder.delta += table[g_encoder.last];
#endif

    //使用状态机来识别按键
    int btn_level = gpio_get_level(EC11_E_PIN);

    if(g_encoder.btn_state){
        g_encoder.ticks++;
    }

    if(btn_level != g_encoder.btn_level){
        g_encoder.debounce_cnt++;
        if(g_encoder.debounce_cnt >= DEBOUNCE_TICKS){
            g_encoder.btn_level = btn_level;
            g_encoder.debounce_cnt = 0;
        }
    }else{
        g_encoder.debounce_cnt = 0;
    }

    switch (g_encoder.btn_state)
    {
    case 0:
        if(g_encoder.btn_level == PIN_ACTIVE){//空状态下，按键按下
            g_encoder.btn_state = 1;//识别到按键按下
            g_encoder.ticks = 0;
            g_encoder.need_report = 1;
            g_encoder.event = Pressed;
        }else{
            //无事件发生
            g_encoder.event = None; 
        }
        break;
    case 1://发生释放或者长按事件
        if(g_encoder.btn_level != PIN_ACTIVE){
            //按键释放事件
            g_encoder.clicks = 1;

            g_encoder.btn_state = 2;//转到双击判断状态
            g_encoder.ticks = 0;
        }else if(g_encoder.ticks > LONG_PRESS_TICKS){
            //发生长按事件
            g_encoder.event = LONG_Pressed;
            g_encoder.need_report = 1;
            g_encoder.btn_state = 4;//转到等待长按释放状态
        }
        break;
    case 2:
        if(g_encoder.btn_level == PIN_ACTIVE){
            g_encoder.btn_state = 3;
            g_encoder.ticks = 0;
        }else if(g_encoder.ticks > SHORT_TICKS){
            if(g_encoder.clicks == 1){
                //发生单击事件
                g_encoder.event = Clicked;
            }else if(g_encoder.clicks == 2){
                //发生双击事件
                g_encoder.event = DoubleClicked;
            }
            g_encoder.need_report = 1;
            g_encoder.btn_state = 0;
        }
        break;
    case 3:
        if(g_encoder.btn_level != PIN_ACTIVE){
            if(g_encoder.ticks < SHORT_TICKS){
                //按键释放事件
                g_encoder.clicks++;
                g_encoder.ticks = 0;
                g_encoder.btn_state = 2;
                g_encoder.event = Released;
                g_encoder.need_report = 1;
            }else{
                g_encoder.btn_state = 0;
            }
        }
        break;
    case 4:
        if(g_encoder.btn_level == PIN_ACTIVE){
            //hold事件
            if(g_encoder.ticks > HOLD_PRESS_TICKS){
                g_encoder.event = Hold;
            }else{
                g_encoder.event = LONG_Pressed;//持续上报按压
            }
            g_encoder.need_report = 1;
        }else{
            //长按释放
            g_encoder.btn_state = 0;
        }
    default:
        break;
    }

    if(g_encoder.need_report){
        putKeyToBuff(&g_evt_buf,g_encoder.event);
        g_encoder.need_report = 0;
    }

}

void encoder_init(void)
{
    gpio_config_t gpio_conf;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask = (1ULL << EC11_A_PIN) | (1ULL << EC11_B_PIN) | (1ULL << EC11_E_PIN);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&gpio_conf);

    memset(&g_encoder,0,sizeof(encoder_t));
    
    keybuffInit(&g_evt_buf);

    /*init state*/
    if(gpio_get_level(EC11_A_PIN) == PIN_ACTIVE){
        g_encoder.last = 3;
    }

    if(gpio_get_level(EC11_B_PIN) == PIN_ACTIVE){
        g_encoder.last ^= 1;
    }

    g_encoder.btn_level = gpio_get_level(EC11_E_PIN);


    esp_timer_create_args_t encoder_timer;
    encoder_timer.arg = NULL;
    encoder_timer.callback = timer_service;
    encoder_timer.dispatch_method = ESP_TIMER_TASK;
    encoder_timer.name = "encoder_timer";
    esp_timer_create(&encoder_timer, &g_encoder_timer_handle);
    esp_timer_start_periodic(g_encoder_timer_handle, TICKS_INTERVAL * 1000U);

}




int16_t get_value(void)
{
    return g_encoder.delta;
}

button_ev_t get_btn_event(void)
{
    button_ev_t event = getKeyFromBuff(&g_evt_buf);
    // ESP_LOGI(TAG,"btn event:%s \n",ev_to_text[event]);
    return event;
}




void keybuffInit(keybuff_t *buff)
{
    memset(buff,0,sizeof(keybuff_t));
}

uint8_t keybuffIsEmpty(keybuff_t *buff)
{
    if(buff->read == buff->write){
        return 1;
    }else{
        return 0;
    }
}

uint8_t keybuffIsFull(keybuff_t *buff)
{
    if((buff->write+1)%BUFF_NUMS == buff->read){
        return 1;
    }else{
        return 0;
    }
}

button_ev_t getKeyFromBuff(keybuff_t *buff)
{
    button_ev_t tempKey = buff->keycode[buff->read];
    if(!keybuffIsEmpty(buff)){
        buff->read = (buff->read+1)%BUFF_NUMS;
        return tempKey;
    }
    return None;
}

void putKeyToBuff(keybuff_t *buff , button_ev_t key)
{
    if(!keybuffIsFull(buff)){
        buff->keycode[buff->write] = key;
        buff->write = (buff->write+1)%BUFF_NUMS;
    }
}
