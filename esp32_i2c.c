#include"driver/i2c.h"


#define I2C_MASTER_SDA_IO    21
#define I2C_MASTER_SCL_IO    22
#define I2C_MASTER_FREQ_HZ   400000

#define ACK_CHECK 1

i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,         // select GPIO specific to your project
    .sda_pullup_en = false,
    .scl_io_num = I2C_MASTER_SCL_IO,         // select GPIO specific to your project
    .scl_pullup_en = false,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,  // select frequency specific to your project
    // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
};

i2c_port_t i2c_master_port = I2C_NUM_0;
i2c_cmd_handle_t i2c_cmd;
void esp32_u8g_i2c_init()
{
    i2c_param_config(i2c_master_port,&conf);

    i2c_driver_install(i2c_master_port,conf.mode,0,0,0);

    i2c_cmd = i2c_cmd_link_create();
    
}

//sla is a 7 bit i2c device address
uint8_t esp32_u8g_i2c_start(uint8_t sla)
{
   i2c_master_start(i2c_cmd);
   i2c_master_write_byte(i2c_cmd,(sla<<1),ACK_CHECK);
   return 1;
}
uint8_t esp32_u8g_i2c_send_byte(uint8_t byte)
{
  i2c_master_write_byte(i2c_cmd,byte,ACK_CHECK);
  return 1;
}
void esp32_u8g_i2c_stop()
{
   i2c_master_stop(i2c_cmd);
   i2c_master_cmd_begin(i2c_master_port,i2c_cmd,0);
   i2c_cmd_link_delete(i2c_cmd);
}
