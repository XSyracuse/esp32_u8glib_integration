/*
  
  u8g_.c
  com interface for arduino (AND atmega) and the SSDxxxx chip (SOLOMON) variant 
  I2C protocol 
  
  Based on code
  Universal 8bit Graphics Library
  
  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
  Special pin usage:
    U8G_PI_I2C_OPTION	additional options
    U8G_PI_A0_STATE	used to store the last value of the command/data register selection
    U8G_PI_SET_A0		1: Signal request to update I2C device with new A0_STATE, 0: Do nothing, A0_STATE matches I2C device
    U8G_PI_SCL		clock line (NOT USED)
    U8G_PI_SDA		data line (NOT USED)
    
    U8G_PI_RESET		reset line (currently disabled, see below)
  Protocol:
    SLA, Cmd/Data Selection, Arguments
    The command/data register is selected by a special instruction byte, which is sent after SLA
    
    The continue bit is always 0 so that a (re)start is equired for the change from cmd to/data mode
*/

#include<Print.h>
#include <U8glib.h>

extern "C" {
void esp32_u8g_i2c_init();
uint8_t esp32_u8g_i2c_start(uint8_t sla);
uint8_t esp32_u8g_i2c_send_byte(uint8_t byte);
void esp32_u8g_i2c_stop();
}
#define SSD1306_ADDR7    (0x78)
#define SSD1306_SLA_CMD  (0x00)
#define SSD1306_SLA_DATA (0x40)

uint8_t u8g_com_ssd_I2C_start_sequence(u8g_t *u8g)
{
  /* are we requested to set the a0 state? */
  if ( u8g->pin_list[U8G_PI_SET_A0] == 0 )
    return 1;

  /*start address*/
  esp32_u8g_i2c_start(SSD1306_ADDR7);

  if ( u8g->pin_list[U8G_PI_SET_A0] == 0 )
  {
    if ( esp32_u8g_i2c_start(SSD1306_SLA_CMD) == 0 )
      return 0;
  }
  else
  {
    if ( esp32_u8g_i2c_start(SSD1306_SLA_DATA) == 0 )
      return 0;
  }

  u8g->pin_list[U8G_PI_SET_A0] = 0;
  return 1;
}

uint8_t u8g_com_HAL_ESP32_ssd_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  uint8_t *ptr;
  switch(msg)
  {
    case U8G_COM_MSG_INIT:

      esp32_u8g_i2c_init();//(u8g->pin_list[U8G_PI_I2C_OPTION]);
      //u8g_com_ssd_I2C_start_sequence(u8g);
      break;
    
    case U8G_COM_MSG_STOP:
      break;

    case U8G_COM_MSG_RESET:
      /* Currently disabled, but it could be enable. Previous restrictions have been removed */
      break;
      
    case U8G_COM_MSG_CHIP_SELECT:
      u8g->pin_list[U8G_PI_A0_STATE] = 0;
      u8g->pin_list[U8G_PI_SET_A0] = 1;		/* force a0 to set again, also forces start condition */
      
      if ( arg_val == 0 )
      {
        /* disable chip, send stop condition */
	esp32_u8g_i2c_stop();
      }
      else
      {
        /* enable, do nothing: any byte writing will trigger the i2c start */
      }
      break;

    case U8G_COM_MSG_WRITE_BYTE:
      
      if ( u8g_com_ssd_I2C_start_sequence(u8g) == 0 )
      {
        esp32_u8g_i2c_stop();
        return 0;
      }
      if ( esp32_u8g_i2c_send_byte(arg_val) == 0 )
      {
        esp32_u8g_i2c_stop();
        return 0;
      }
      // u8g_i2c_stop();
      break;
    
    case U8G_COM_MSG_WRITE_SEQ:
      //u8g->pin_list[U8G_PI_SET_A0] = 1;
      if (u8g_com_ssd_I2C_start_sequence(u8g) == 0) 
      {
        esp32_u8g_i2c_stop();
        return 0;
      }

      ptr = (uint8_t *)arg_ptr;
      while (arg_val > 0) {
        if (esp32_u8g_i2c_send_byte(*ptr++) == 0) 
        {
          esp32_u8g_i2c_stop();
          return 0;
        }
        arg_val--;
      }
      
      // u8g_i2c_stop();
      break;

    case U8G_COM_MSG_WRITE_SEQ_P:
      //u8g->pin_list[U8G_PI_SET_A0] = 1;
      if (u8g_com_ssd_I2C_start_sequence(u8g) == 0) 
      {
        esp32_u8g_i2c_stop();
        return 0;
      }

      ptr = (uint8_t *)arg_ptr;
      while (arg_val > 0) {
        if (esp32_u8g_i2c_send_byte(u8g_pgm_read(ptr)) == 0)
        {
          esp32_u8g_i2c_stop();
          return 0;
        }
        ptr++;
        arg_val--;
        
      }
      // u8g_i2c_stop();
      break;
      
    case U8G_COM_MSG_ADDRESS:                     /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
      u8g->pin_list[U8G_PI_A0_STATE] = arg_val;
      u8g->pin_list[U8G_PI_SET_A0] = 1;		/* force a0 to set again */
  
      break;
    default:
      break;
  }//switch

  return 1;
}


