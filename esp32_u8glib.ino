#include<U8glib.h>
#include<Wire.h>
#define TARGET_ESP32

/*
#ifdef TARGET_ESP32
#pragma message "taget esp32" 
#ifdef u8g_i2c_init
#undef u8g_i2c_init
void u8g_i2c_init(const uint8_t clock_options){

  Wire.begin();
}
#endif

#ifdef u8g_i2c_start
#undef u8g_i2c_start
uint8_t u8g_i2c_start(uint8_t sla){
  Wire.beginTransmission(sla);  
}
#endif

#ifdef u8g_i2c_send_byte
#undef u8g_i2c_send_byte
uint8_t u8g_i2c_send_byte(uint8_t data){
  Wire.write(data);
}
#endif

#ifdef u8g_i2c_stop
#undef u8g_i2c_stop
void u8g_i2c_stop(){
  Wire.endTransmission();
}
#endif

#endif //TARGET
*/
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 

#define HEIGHT 64
#define WIDTH 128

// communication procedure
#define U8G_COM_SSD_I2C_HAL         u8g_com_HAL_ESP32_ssd_i2c_fn
extern uint8_t u8g_com_HAL_ESP32_ssd_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr);

// device procedure
uint8_t u8g_dev_ssd1306_128x64_2x_2_wire_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {}

//device memory
uint8_t u8g_dev_ssd1306_128x64_2x_i2c_2_wire_buf[WIDTH*2] U8G_NOCOMMON ;
u8g_pb_t u8g_dev_ssd1306_128x64_2x_i2c_2_wire_pb = { {16, HEIGHT, 0, 0, 0},  WIDTH, u8g_dev_ssd1306_128x64_2x_i2c_2_wire_buf};

//define dev
u8g_dev_t u8g_dev_ssd1306_128x64_2x_i2c_2_wire = { u8g_dev_ssd1306_128x64_2x_2_wire_fn, &u8g_dev_ssd1306_128x64_2x_i2c_2_wire_pb, U8G_COM_SSD_I2C_HAL };



class ssd : public U8GLIB {
public:
  ssd(uint8_t options = U8G_I2C_OPT_NONE)
  : U8GLIB(&u8g_dev_ssd1306_128x64_2x_i2c_2_wire, options)
  {  }
};

ssd u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);
void setup() {
  // put your setup code here, to run once:
  
}
void u8g_box_frame(uint8_t a) {
  u8g.drawStr( 0, 0, "drawBox");
  u8g.drawBox(5,10,20,10);
  u8g.drawBox(10+a,15,30,7);
  u8g.drawStr( 0, 30, "drawFrame");
  u8g.drawFrame(5,10+30,20,10);
  u8g.drawFrame(10+a,15+30,30,7);
}
void loop() {
  // put your main code here, to run repeatedly:
  u8g_box_frame(10);
}
