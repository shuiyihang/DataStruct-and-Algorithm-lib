#pragma once
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define BUFF_NUMS     10

typedef enum
{
    Pressed,
    LONG_Pressed,
    Hold,

    Released,
    LONG_Press_released,

    Clicked,
    DoubleClicked,
    None, 
}button_ev_t;


typedef struct encoder
{
    volatile int16_t last;
    volatile int16_t delta;
    uint8_t btn_state;
    uint8_t clicks;//一个时间段内单击次数
    uint16_t ticks;
    uint8_t btn_level:1;//press or release
    uint8_t need_report:1;
    uint8_t debounce_cnt;//按键去抖次数
    button_ev_t event;
    
}encoder_t;

typedef struct keybuff
{
    button_ev_t keycode[BUFF_NUMS];//键值
    uint8_t write;
    uint8_t read;
}keybuff_t;


#define EC11_A_PIN  3
#define EC11_B_PIN  9
#define EC11_E_PIN  10

#define PIN_ACTIVE  0

#define TICKS_INTERVAL 5

#define ENC_NORMAL        (1 << 1)   // use Peter Danneger's decoder
#define ENC_CUSTOM         (1 << 2)   // use Table-based decoder


// #define ENC_DECODER     ENC_NORMAL
#define ENC_DECODER     ENC_CUSTOM

#define BUTTON_HOLD_PRESS_TIME_MS   1600
#define BUTTON_LONG_PRESS_TIME_MS   1200
#define BUTTON_SHORT_PRESS_TIME_MS  300
#define DEBOUNCE_TICKS  4
#define HOLD_PRESS_TICKS    (BUTTON_HOLD_PRESS_TIME_MS /TICKS_INTERVAL)
#define LONG_PRESS_TICKS    (BUTTON_LONG_PRESS_TIME_MS /TICKS_INTERVAL)
#define SHORT_TICKS         (BUTTON_SHORT_PRESS_TIME_MS /TICKS_INTERVAL)
void encoder_init(void);
int16_t get_value(void);
button_ev_t get_btn_event(void);



void keybuffInit(keybuff_t *buff);

uint8_t keybuffIsEmpty(keybuff_t *buff);

uint8_t keybuffIsFull(keybuff_t *buff);

button_ev_t getKeyFromBuff(keybuff_t *buff);

void putKeyToBuff(keybuff_t *buff , button_ev_t key);
