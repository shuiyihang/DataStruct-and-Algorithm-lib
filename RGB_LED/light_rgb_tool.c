
#include "light_rgb_core.h"
u32_t RGB565toGRB888(u16_t grb)
{
	u8_t p[3];
	u32_t ret_vel =0;
	p[0] = (grb&0x1F) <<3;//b
	p[1] = ((grb>>5) &0x3F) <<2;//g
	p[2] = ((grb>>11) &0x1F) <<3;//r
	/*补偿优化*/
	p[0] |= (grb&7);
	p[1] |= ((grb>>5)&3);
	p[2] |= ((grb>>11)&7);


	ret_vel = p[1]<<16 | p[2] << 8 | p[0];
	return ret_vel;
}


void spi_dev_init(struct spi_config *cfg)
{
    cfg->dev = device_get_binding(CONFIG_SPI_1_NAME);
    cfg->frequency = 0;
    cfg->operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) |SPI_HOLD_ON_CS;
    cfg->vendor = 0;
    cfg->slave = 0;
    cfg->cs = NULL;
}