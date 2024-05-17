
#include "stream.h"
#include "bt_eg_api.h"
#include <os_common_api.h>
#include <mem_manager.h>

#include "lx_msg_deal.h"
/**
 * 接收使用的非环形缓冲，申请的空间比较大一些
*/

#define MDA_SERVICE_UUID BT_UUID_DECLARE_128(0x01,0x3B,0xF5,0xDA,0x00,0x00,\
                                                0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB)
#define MDA_CHA_1TX_UUID BT_UUID_DECLARE_16(0x8888)
#define MDA_CHA_1RX_UUID BT_UUID_DECLARE_16(0x7777)

static os_mutex rw_table_mutex;// table表互斥使用，确保不会任意时刻断开释放后不会再使用(使用中，断开ble，一直等待使用释放)

struct lx_ble_rw_table
{
    u8_t *txbuf;
    u8_t *rxbuf;
    u16_t tx_rofs;
    u16_t tx_wofs;
    u16_t rx_rofs;
    u16_t rx_wofs;
};

struct lx_ble_rw_table *rw_table = NULL;

static bool ble_use = true;

// os_delayed_work app_cmd_disc_delaywork;

static os_delayed_work lx_connect_delaywork;
static os_delayed_work lx_disconnect_delaywork;
static os_delayed_work lx_run_delaywork;


u8_t* g_temp_buf = NULL;

static io_stream_t lx_ble_stream;


static u8_t spp_uuid[] = {0x0C,0xF1,0x2D,0x31,0xFA,0xC3,0x45,0x53,0xBD,0x80,0xD6,0x83,0x2E,0x7B,0x33,0x98};
static struct le_gatt_attr lx_gatt_attr[] = {
    BT_GATT_PRIMARY_SERVICE(MDA_SERVICE_UUID),
	BT_GATT_CHARACTERISTIC(MDA_CHA_1TX_UUID,  ATT_READ | ATT_NOTIFY | ATT_EXTENDED_PROPERTIES, NULL, (DEFAULT_LOCAL_ATT_MTU - 3), NULL),
	BT_GATT_CLIENT_CHRC_CONFIG(GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_INDICATIONS_ENABLED, NULL),
	BT_GATT_CHARACTERISTIC(MDA_CHA_1RX_UUID, ATT_WRITE | ATT_WRITE_NORSP, NULL, (DEFAULT_LOCAL_ATT_MTU - 3), NULL)
};

const u8_t send_head[] =    {0x08, 0xee, 0x00, 0x00, 0x00};


extern void bt_manager_ble_adv_onoff(u8_t onoff);
/*

*/
static int lx_match_msg_head(u8_t *buf,u16_t buf_len,u16_t *head_len,u16_t *payloadLen)
{

    if(!memcmp(buf,send_head,sizeof(send_head))){
        if(buf_len < 9){//buf[7]和buf[8]还没有接收到
            *head_len = 1;
            *payloadLen = LX_RX_BUF_SIZE;//目的是让break掉
        }else{
            *head_len = sizeof(send_head);
            *payloadLen = (buf[7]|buf[8]<<8) - *head_len;
            // SYS_LOG_INF("normal\n");
        }
        return TRUE;

    }else{
        // SYS_LOG_INF("not finish\n");
        return FALSE;
    }
}

static int lx_sum_check(u8_t *buf,u16_t len)
{
    u8_t sum=0;
    for(int i = 0; i<len-1; i++){
        sum += buf[i];
    }
    // SYS_LOG_INF("sum:%2x,buf[len-1]:%2x\n",sum,buf[len-1]);
    if(sum == buf[len-1]){
        return TRUE;
    }
    return FALSE;
}



extern void lx_app_msg_deal(u8_t *buf, u16_t len);

//非循环，所以也就是    wofs > rofs
static void lx_rx_process(void)
{
    u16_t off_len,payload_len;
    u16_t unuse_len;

    os_mutex_lock(&rw_table_mutex, K_FOREVER);
    unuse_len = LX_RX_BUF_SIZE - rw_table->rx_wofs;
    int len = stream_read(lx_ble_stream,&rw_table->rxbuf[rw_table->rx_wofs],unuse_len);
    if(len <= 0)
    {
        goto _err;
    }
    rw_table->rx_wofs += len;

    print_buffer(&rw_table->rxbuf[rw_table->rx_rofs], 1, rw_table->rx_wofs, 16, 0);

    while(rw_table->rx_wofs - rw_table->rx_rofs >= LX_PACK_HEAD){//接收到数据至少大于 标识包头
        if(lx_match_msg_head(&rw_table->rxbuf[rw_table->rx_rofs],\
            rw_table->rx_wofs - rw_table->rx_rofs, &off_len, &payload_len)){

            // SYS_LOG_INF("rx_wofs:%d,rx_rofs%d\n",rw_table->rx_wofs,rw_table->rx_rofs);
            if(rw_table->rx_wofs - rw_table->rx_rofs >= (off_len+payload_len)){
                //数据校验
                if(lx_sum_check(&rw_table->rxbuf[rw_table->rx_rofs],(off_len+payload_len))){
                    //消息处理
                    lx_app_msg_deal(&rw_table->rxbuf[off_len + rw_table->rx_rofs],payload_len);
                    rw_table->rx_rofs += (off_len+payload_len);
                }else{
                    rw_table->rx_wofs = 0;
                }   
            }else{//没有拿到完整包
                break;
            }

        }else{
            //丢掉所有数据，退出，只要用户不是一直在发送，即使出错之后，也是能恢复正常的
            //没有必要在错误里面寻找正确的头
            rw_table->rx_wofs = 0;
            break;
        }
    }

    if(rw_table->rx_rofs){
        rw_table->rx_wofs -= rw_table->rx_rofs;
        if(rw_table->rx_wofs)memcpy(rw_table->rxbuf,&rw_table->rxbuf[rw_table->rx_rofs],rw_table->rx_wofs);
        rw_table->rx_rofs = 0;
    }

_err:
    os_mutex_unlock(&rw_table_mutex);
    return;
}




//从发送缓冲里拿数据
static void lx_tx_process(void)
{
    u16_t pkg_len;
    u16_t p_len;
    os_mutex_lock(&rw_table_mutex, K_FOREVER);
    pkg_len = rw_table->txbuf[rw_table->tx_rofs]<<8 | rw_table->txbuf[(rw_table->tx_rofs+1)%LX_TX_BUF_SIZE];
    rw_table->tx_rofs = (rw_table->tx_rofs+2)%LX_TX_BUF_SIZE;//跳过两个长度字节

    if(rw_table->tx_rofs + pkg_len > LX_TX_BUF_SIZE){
        p_len = LX_TX_BUF_SIZE - rw_table->tx_rofs;
        memcpy(g_temp_buf,&rw_table->txbuf[rw_table->tx_rofs],p_len);
        memcpy(&g_temp_buf[p_len],rw_table->txbuf,pkg_len - p_len);
    }else{
        memcpy(g_temp_buf,&rw_table->txbuf[rw_table->tx_rofs],pkg_len);
    }
    rw_table->tx_rofs = (rw_table->tx_rofs + pkg_len)%LX_TX_BUF_SIZE;

    print_buffer(g_temp_buf, 1, pkg_len, 16, 0);
    stream_write(lx_ble_stream,g_temp_buf,pkg_len);
    os_mutex_unlock(&rw_table_mutex);
}


void lx_put_pkg_to_ringbuf(u8_t *pkg, u16_t len)
{
    u16_t p_len;
    if(rw_table == NULL){
        goto _err1;
    }
    os_mutex_lock(&rw_table_mutex, K_FOREVER);

    if(rw_table->tx_wofs + len + 2 > LX_TX_BUF_SIZE 
        && (rw_table->tx_wofs + len + 2)%LX_TX_BUF_SIZE > rw_table->tx_rofs){//环形缓冲未发送数据防覆盖
        SYS_LOG_INF("======you need turn up ringbuff=====\n");
        goto _err;
    }

    rw_table->txbuf[rw_table->tx_wofs] = (u8_t)(len>>8);
    rw_table->tx_wofs = (rw_table->tx_wofs+1) % LX_TX_BUF_SIZE;
    rw_table->txbuf[rw_table->tx_wofs] = (u8_t)(len);
    rw_table->tx_wofs = (rw_table->tx_wofs+1) % LX_TX_BUF_SIZE;

    if(rw_table->tx_wofs + len >= LX_TX_BUF_SIZE){
        p_len = LX_TX_BUF_SIZE - rw_table->tx_wofs;
        memcpy(&rw_table->txbuf[rw_table->tx_wofs], pkg, p_len);
        memcpy(rw_table->txbuf,&pkg[p_len],len-p_len);
        rw_table->tx_wofs = len-p_len;
    }else{
        memcpy(&rw_table->txbuf[rw_table->tx_wofs], pkg, len);
        rw_table->tx_wofs += len;
    }

_err:
    os_mutex_unlock(&rw_table_mutex);
_err1:
    return;
}

static void lx_connect_proc(os_work *work)
{
    int ret = stream_open(lx_ble_stream, MODE_IN_OUT);
    if(ret == 0){
        rw_table = mem_malloc(sizeof(struct lx_ble_rw_table));
        if(!rw_table)
            return;
        memset(rw_table,0,sizeof(struct lx_ble_rw_table));
        rw_table->txbuf = mem_malloc(LX_TX_BUF_SIZE);
        if(!rw_table->txbuf){
            return;
        }
        rw_table->rxbuf = mem_malloc(LX_RX_BUF_SIZE);

        if(!rw_table->rxbuf){
            mem_free(rw_table->txbuf);
            mem_free(rw_table);
            rw_table = NULL;
        }
        os_delayed_work_submit(&lx_run_delaywork, 0);//跑进消息的收发里面
    }
}

static void lx_disconnect_proc(os_work *work)
{
    stream_close(lx_ble_stream);//手机主动断开不需要等待buf为空
    os_mutex_lock(&rw_table_mutex, K_FOREVER);
    if(rw_table){
        mem_free(rw_table->txbuf);
        mem_free(rw_table->rxbuf);
        mem_free(rw_table);
        rw_table = NULL;
    }
    os_mutex_unlock(&rw_table_mutex);
    bt_manager_ble_adv_onoff(1);
}

// void public_disconnect_ble(os_work *work)
// {
//     if(rw_table->tx_rofs != rw_table->tx_wofs){
//         os_delayed_work_submit(&app_cmd_disc_delaywork, 30);//跑进消息的收发里面
//     }else{
//         SYS_LOG_INF("=======disconnect ble========\n");
//         stream_close(lx_ble_stream);//手机主动断开不需要等待buf为空
//         os_mutex_lock(&rw_table_mutex, K_FOREVER);
//         if(rw_table){
//             mem_free(rw_table->txbuf);
//             mem_free(rw_table->rxbuf);
//             mem_free(rw_table);
//             rw_table = NULL;
//         }
//         os_mutex_unlock(&rw_table_mutex);
//     }
// }

static void lx_run_proc(os_work *work)
{
    if(rw_table && ble_use){
        lx_rx_process();
        if(rw_table->tx_rofs != rw_table->tx_wofs){//环形缓冲非空
            lx_tx_process();
        }
        os_delayed_work_submit(&lx_run_delaywork, LX_RXTX_INTERVAL);//跑进消息的收发里面
    }
}


static void lx_sppble_connect(BOOL connect)
{
    if(connect){
        bt_manager_ble_adv_onoff(0);
        os_delayed_work_submit(&lx_connect_delaywork, 10);
    }else{
        os_delayed_work_submit(&lx_disconnect_delaywork, 0);
    }
}

void bt_engine_close_ble(void)
{
    ble_use = false;
}

void lx_ble_app_init(void)
{
    struct sppble_stream_init_param init_param;

    os_mutex_init(&rw_table_mutex);
    init_param.spp_uuid = spp_uuid;
    init_param.le_gatt_attr = &lx_gatt_attr;
    init_param.attr_size = ARRAY_SIZE(lx_gatt_attr);
	init_param.tx_attr = &lx_gatt_attr[1];
	init_param.ccc_attr = &lx_gatt_attr[2];
	init_param.rx_attr = &lx_gatt_attr[3];
	init_param.connect_cb = lx_sppble_connect;
	init_param.read_timeout = K_NO_WAIT;	/* K_FOREVER, K_NO_WAIT,  K_MSEC(ms) */
	init_param.write_timeout = K_NO_WAIT;

    g_temp_buf = mem_malloc(sizeof(u8_t) * PKG_MAX_LEN);
    if(!g_temp_buf){
        SYS_LOG_INF("=======malloc failed!========\n");
    }

    os_delayed_work_init(&lx_connect_delaywork, lx_connect_proc);
	os_delayed_work_init(&lx_disconnect_delaywork, lx_disconnect_proc);
	os_delayed_work_init(&lx_run_delaywork, lx_run_proc);

    // os_delayed_work_init(&app_cmd_disc_delaywork,public_disconnect_ble);

    lx_ble_stream = stream_create(TYPE_SPPBLE_STREAM, &init_param);
    SYS_LOG_INF("=======lx_ble_app_init ok========\n");
}