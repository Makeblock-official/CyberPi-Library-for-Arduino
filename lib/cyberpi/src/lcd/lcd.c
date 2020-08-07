#include "lcd.h"
#include "GT30L24A3W.h"


#define CONFIG_LIBRARY_SPI_NUM                (1)
#define CONFIG_LIBRARY_SPI_CLOCK              (20000000)
#define CONFIG_LIBRARY_MOSI_GPIO              (2)
#define CONFIG_LIBRARY_MISO_GPIO              (26) 
#define CONFIG_LIBRARY_CLK_GPIO               (4)

#define CONFIG_LIBRARY_CS_GPIO                (27)
#define CONFIG_LIBRARY_BCKL_ACTIVE_LEVEL      (1)  
static spi_device_handle_t spi_wr_library;

void library_spi_cmd(spi_device_handle_t spi, const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t = {
        .length = 8,                    // Command is 8 bits
        .tx_buffer = &cmd,              // The data is the cmd itself
    };
    ret = _lcd_spi_send(spi, &t);       // Transmit! 
}

uint8_t library_get_data(spi_device_handle_t spi, uint8_t *data_out, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length= 8 * len;
    t.rx_buffer = data_out;
    ret = _lcd_spi_send(spi, &t);

    // assert( ret == ESP_OK );

    return 0;
}
unsigned char r_dat_bat(unsigned long  ReadAddr,unsigned int  NumByteToRead,unsigned char* pBuffer)
{
  unsigned long j = 0;
  gpio_set_level(CONFIG_LIBRARY_CS_GPIO, 0);
  
  library_spi_cmd(spi_wr_library, 0x03);  //发送命令
  library_spi_cmd(spi_wr_library, (unsigned char)(ReadAddr >> 16));  //发送地址
  library_spi_cmd(spi_wr_library, (unsigned char)(ReadAddr >> 8));
  library_spi_cmd(spi_wr_library, (unsigned char)(ReadAddr >> 0));

  library_get_data(spi_wr_library, pBuffer, NumByteToRead);
  gpio_set_level(CONFIG_LIBRARY_CS_GPIO, 1);
  return 0;  
}

static uint32_t library_spi_init(spi_device_handle_t *spi_wr_dev_library, int dma_chan)
{
    spi_device_interface_config_t devcfg = {
        // Use low speed to read ID.
        .clock_speed_hz = 4 * 1000 * 1000,        //Clock out frequency
        .mode = 0,                                //SPI mode 0
        .spics_io_num = CONFIG_LIBRARY_CS_GPIO,     //CS pin
        .queue_size = 7,                          //We want to be able to queue 7 transactions at a time
    };

    // Use high speed to write library
    devcfg.clock_speed_hz = CONFIG_LIBRARY_SPI_CLOCK;
    //devcfg.flags = SPI_DEVICE_HALFDUPLEX;
    esp_err_t r1 = spi_bus_add_device((spi_host_device_t)CONFIG_LIBRARY_SPI_NUM, &devcfg, spi_wr_dev_library);

    return ESP_OK;
}
static void mb_library_config_t()
{
  library_spi_init( &spi_wr_library, 1);
}
int get_utf8_data(uint32_t letter, uint8_t word_size, uint8_t *map, bool *elongate,uint8_t *width,uint8_t* height)
{
  uint8_t gb_msb,gb_lsb;
  unsigned short Unicode = 0;
  unsigned long GB_Code = 0x0000;
  int use_word_size = 0;

  if((letter >= 0x00) && ( letter <= 0x7F))      //ascii码
  {
    use_word_size = word_size;
    ASCII_GetData(letter, use_word_size, map);
    
    if(use_word_size == ASCII_12_A ||
       use_word_size == ASCII_16_A ||  
       use_word_size == ASCII_24_B || 
       use_word_size == ASCII_32_B)
    {
      *elongate = true;
      if(width!=0)*width = map[1];
    }

  } 
  //法语、德语、西班牙语、意大利、荷兰(拉丁文) 
  else if((letter >= 0x0020 && letter <= 0x007F) || (letter >= 0x00A0 && letter <= 0x017F) || (letter >= 0x01A0 && letter <= 0x1CF) \
   || (letter >= 0x01F0 && letter <= 0x01FF) || (letter >= 0x0210 && letter <= 0x021F) || (letter >= 0x1EA0 && letter <= 0x1EFF)) 
  {
    if(word_size <= ASCII_8X16)
    {
      LATIN_8X16_GetData(letter, map);
      use_word_size = ASCII_8X16;
    }
    else if(word_size == ASCII_16_A)
    {
      LATIN_16_GetData(letter, map);
      // *elongate = true;
      use_word_size = ASCII_16_A;
      // if(width!=0)*width = map[1];
    }
    else if(word_size >= ASCII_12X24_A)
    {
      LATIN_12X24_GetData(letter, map);
      use_word_size = ASCII_12X24_A;
    }
  }
  //俄语(西尔里文)
  else if((letter >= 0x0400 && letter <= 0x045F) || (letter >= 0x0490 && letter <= 0x04FF)) 
  {
    if(word_size <= ASCII_8X16)
    {
      CYRILLIC_8X16_GetData(letter, map);
      use_word_size = ASCII_8X16;
    }
    else if(word_size == ASCII_16_A)
    {
      CYRILLIC_16_GetData(letter, map);
      // *elongate = true;
      use_word_size = ASCII_16_A;
      // if(width!=0)*width = map[1];
    }
    else if(word_size >= ASCII_12X24_A)
    {
      CYRILLIC_12X24_GetData(letter, map);
      use_word_size = ASCII_12X24_A;
    }
  }
  // 希腊文
  else if(letter >= 0x0370 && letter <= 0x03CF)
  {
    if(word_size <= ASCII_8X16)
    {
      GREECE_8X16_GetData(letter, map);
      use_word_size = ASCII_8X16;
    }
    else if(word_size == ASCII_16_A)
    {
      GREECE_16_GetData(letter, map);
      // *elongate = true;
      use_word_size = ASCII_16_A;
      // if(width!=0)*width = map[1];
    }
    else if(word_size >= ASCII_12X24_A)
    {
      GREECE_12X24_GetData(letter, map);
      use_word_size = ASCII_12X24_A;
    }    
  }
  // 希伯来文
  else if((letter >= 0x0590 && letter <= 0x05FF) || (letter >= 0xFB1D && letter <= 0xFB4F))
  {
    if(word_size <= ASCII_8X16)
    {
      HEBREW_8X16_GetData(letter, map);
      use_word_size = ASCII_8X16;
    }
    else if(word_size >= ASCII_12X24_A)
    {
      HEBREW_12X24_GetData(letter, map);
      use_word_size = ASCII_12X24_A;
    }  
  }
  // 阿拉伯文
  else if((letter >= 0x0600 && letter <= 0x06FF) || (letter >= 0xFB50 && letter <= 0xFBFF) || (letter >= 0xFE70 && letter <= 0xFEFF))
  {
    if(word_size <= ASCII_16_A)
    {
      ALB_16_GetData(letter, map);
      use_word_size = ASCII_16_A;
      // if(width!=0)*width = map[1];
      // *elongate = true;
    }
    else
    {
      ALB_24_GetData(letter, map);
      use_word_size = ASCII_24_B;
      if(width!=0)*width = map[1];
      *elongate = true;
    }    
  }
  // 泰文
  else if(letter >= 0x0E00 && letter <= 0x0E5F)
  {
    if(word_size <= ASCII_8X16)
    {
      THAILAND_8X16_GetData(letter, map);
      use_word_size = ASCII_8X16;
    }
    else if(word_size >= ASCII_12X24_A)
    {
      THAILAND_16X24_GetData(letter, map);
      use_word_size = ASCII_12X24_A;
    }  
  }
  // 中、日、韩
  else if((letter >= 0x00A0 && letter <= 0x0451) || (letter >= 0x2010  && letter <= 0x2642) || (letter >= 0x3000  && letter <= 0x33D5) \
    || (letter >= 0x4E00  && letter <= 0x9FA5) || (letter >= 0xFE30  && letter <= 0xFE6B) || (letter >= 0xFF01  && letter <= 0xFF5E) \
    || (letter >= 0xFFE0  && letter <= 0xFFE5) || (letter >= 0xF92C  && letter <= 0xFA29) || (letter >= 0xE816  && letter <= 0xE864) \
    || (letter >= 0x2E81  && letter <= 0x2ECA) || (letter >= 0x4947  && letter <= 0x49b7) || (letter >= 0x4C77  && letter <= 0x4DAE) \
    || (letter >= 0x3447  && letter <= 0x3CE0) || (letter >= 0x4056  && letter <= 0x478D) || (letter >= 0x0020 && letter < 0x07FF) || (letter >= 0x2000 && letter < 0x27FF) || (letter >= 0x3000 && letter < 0x30FF) \
    || (letter >= 0x3200 && letter < 0x33FF) || (letter >= 0x4E00 && letter < 0x9FFF) || (letter >= 0xFE00 && letter < 0xFFFF) \
    || (letter >= 0x2B05 && letter < 0x2B07) || (letter >= 0x00A1 && letter <= 0x0451) || (letter >= 0x2015 && letter <= 0x266D) || (letter >= 0x3000 && letter <= 0x30FF) \
    || (letter >= 0xAC00 && letter <= 0xD79D) || (letter >= 0xF900 && letter <= 0xFFFF))
  {
    GB_Code = U2G(letter);
    gb_msb = GB_Code >> 8;
    gb_lsb = GB_Code & 0x00ff;

    if((gb_lsb == 0x7F) || (gb_msb >= 0xA1 && gb_msb <= 0xA3 && gb_lsb >= 0xA1) || (gb_msb == 0xA6 && gb_lsb >= 0xA1) \
      || (gb_msb == 0xA9 && gb_lsb >= 0xA1) || (gb_msb >= 0xB0 && gb_msb <= 0xF7 && gb_lsb >= 0xA1) || (gb_msb < 0xA1 && gb_msb >= 0x81 && gb_lsb >= 0x40) || (gb_msb >= 0xAA && gb_lsb < 0xA1))
    {
      if(word_size <= ASCII_12_A &&
         ((gb_msb==0xA9UL && gb_lsb >=0xA4UL) ||
         (gb_msb >=0xA1UL && gb_msb <= 0XA3UL && gb_lsb >=0xA1UL) ||
         (gb_msb >=0xB0UL && gb_msb <= 0xF7UL && gb_lsb >=0xA1UL)))
      {
        gt_12_GetData(gb_msb, gb_lsb, map);
        use_word_size = ASCII_12_A;
        if(width!=0)*width = map[1];
      }
      else if(word_size == ASCII_16_A &&
             ((gb_msb==0xA9UL && gb_lsb >=0xA4UL) ||
             (gb_msb >=0xA1UL && gb_msb <= 0XA3UL && gb_lsb >=0xA1UL) ||
             (gb_msb >=0xB0UL && gb_msb <= 0xF7UL && gb_lsb >=0xA1UL)))
      {
        gt_16_GetData(gb_msb, gb_lsb, map);
        use_word_size = ASCII_16_A;
        // if(width!=0)*width = map[1];
      }
      else// if(word_size <= ASCII_24_B)
      {
        GBK_24_GetData(gb_msb, gb_lsb, map);
        use_word_size = ASCII_24_B;
      }
    }
    else
    {
      GB_Code = U2J(letter);
      gb_msb = GB_Code >> 8;
      gb_lsb = GB_Code & 0x00ff;
      if(((gb_msb <= 0x85 && gb_msb >= 0x01) && (gb_lsb <= 0x94 && gb_lsb >= 0x01)) || ((gb_msb <= 0x89 && gb_msb >= 0x88) && (gb_lsb <= 0x94 && gb_lsb >= 0x01)))
      {
        if(word_size <= ASCII_8X16)
        {
          JIS0208_16X16_GetData(gb_msb, gb_lsb, map);
          use_word_size = ASCII_16_A;
          // *elongate = true;
        }
        else if(word_size == ASCII_16_A)
        {
          JIS0208_16X16_GetData(gb_msb, gb_lsb, map);
          use_word_size = ASCII_16_A;
          // if(width!=0)*width = map[1];
          // *elongate = true;
        }
        else if(word_size >= ASCII_24_B)
        {
          JIS0208_24X24_GetData(gb_msb, gb_lsb, map);
          use_word_size = ASCII_24_B;
        } 
      }
      else
      {
        GB_Code = U2K(letter);
        gb_msb = GB_Code >> 8;
        gb_lsb = GB_Code & 0x00ff;
        if((gb_msb >= 0xA1 && gb_msb < 0xAD && gb_lsb >= 0xA1) || (gb_msb >= 0xB0 && gb_msb <= 0xC8 && gb_lsb >= 0xA1))
        {
          if(word_size <= ASCII_16_A)
          {
            KSC5601_F_16_GetData(gb_msb, gb_lsb, map);
            use_word_size = ASCII_16_A;
            // if(width!=0)*width = map[1];
            // *elongate = true;
          }
          else if(word_size >= ASCII_24_B)
          {
            KSC5601_F_24_GetData(gb_msb, gb_lsb, map);
            use_word_size = ASCII_24_B;
          }   
        }
      }
    } 
  }
  if(width!=0&&height!=0)
  {
    switch(use_word_size)
    {
      case ASCII_5X7:
      {
        *width = 5;
        *height = 7;
      }
      break;
      case ASCII_7X8:
      {
        *width = 7;
        *height = 8;
      }
      break;
      case ASCII_6X12:
      {
        *width = 6;
        *height = 12;
      }
      break;
      case ASCII_12_A:
      {
        *width = 12;
        *height = 12;
      }
      break;
      case ASCII_8X16:
      {
        *width = 8;
        *height = 16;
      }
      break;
      case ASCII_12X24_A:
      {
        *width = 12;
        *height = 24;
      }
      break;
      case ASCII_12X24_P:
      {
        *width = 12;
        *height = 24;
      }
      break;
      case ASCII_16X32:
      {
        *width = 16;
        *height = 32;
      }
      break;
      case ASCII_16_A:
      {
        *width = 16;
        *height = 16;
      }
      break;
      case ASCII_24_B:
      {
        *width = 24;
        *height = 24;
      }
      break;
      case ASCII_32_B:
      {
        *width = 32;
        *height = 32;
      }
      break;
    }
  }
  return use_word_size;
}

lcd_info_t lcd_info = 
{
  .height = LCD_TFTHEIGHT,
  .width  = LCD_TFTWIDTH,
  .dma_mode = true,
  .dma_buf_size = SPI_DMA_BUFFER_SIZE,
  .dma_chan = SPI_DMA_CHANNEL,
  .spi_wr = NULL,
  .lcd_conf = 
  {
    .lcd_model = LCD_MOD_AUTO_DET,
    .pin_num_miso = CONFIG_LCD_MISO_GPIO,
    .pin_num_mosi = CONFIG_LCD_MOSI_GPIO,
    .pin_num_clk = CONFIG_LCD_CLK_GPIO,
    .pin_num_cs = CONFIG_LCD_CS_GPIO,
    .pin_num_dc = CONFIG_LCD_DC_GPIO,
    .pin_num_rst = CONFIG_LCD_RESET_GPIO,
    .pin_num_bckl = CONFIG_LCD_BL_GPIO,
    .clk_freq = CONFIG_LCD_SPI_CLOCK,
    .rst_active_level = 0,
    .bckl_active_level = CONFIG_LCD_BCKL_ACTIVE_LEVEL,
    .spi_host = (spi_host_device_t)CONFIG_LCD_SPI_NUM,
    .init_spi_bus = true,
  }
};

SemaphoreHandle_t _spi_mux = NULL;

const lcd_init_cmd_t st7735_init_cmds[] = 
{
  // software reset with delay
  {ST77XX_SWRESET, {0}, ST_CMD_DELAY},
  // Out of sleep mode with delay
  {ST77XX_SLPOUT, {0}, ST_CMD_DELAY},
  // Framerate ctrl - normal mode. Rate = fosc/(1x2+40) * (LINE+2C+2D)
  {ST7735_FRMCTR1, {0x01, 0x2C, 0x2D}, 3},
  // Framerate ctrl - idle mode.  Rate = fosc/(1x2+40) * (LINE+2C+2D)
  {ST7735_FRMCTR2, {0x01, 0x2C, 0x2D}, 3},
  // Framerate - partial mode. Dot/Line inversion mode
  {ST7735_FRMCTR3, {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}, 6},
  // Display inversion ctrl: No inversion
  {ST7735_INVCTR, {0x07}, 1},
  // Power control1 set GVDD: -4.6V, AUTO mode.
  {ST7735_PWCTR1, {0xA2, 0x02, 0x84}, 3},
  // Power control2 set VGH/VGL: VGH25=2.4C VGSEL=-10 VGH=3 * AVDD
  {ST7735_PWCTR2, {0xC5}, 1},
  // Power control3 normal mode(Full color): Op-amp current small, booster voltage
  {ST7735_PWCTR3, {0x0A, 0x00}, 2},
  // Power control4 idle mode(8-colors): Op-amp current small & medium low
  {ST7735_PWCTR4, {0x8A, 0x2A}, 2},
  // Power control5 partial mode + full colors
  {ST7735_PWCTR5, {0x8A, 0xEE}, 2},
  // VCOMH VoltageVCOM control 1: VCOMH=0x0E=2.850
  {ST7735_VMCTR1, {0x0E}, 1},
  // Display Inversion Off
  {ST77XX_INVOFF, {1}, 1},
  // Memory Data Access Control: top-bottom/left-right refresh
  {ST77XX_MADCTL, {0xA8}, 1},
  // Color mode, Interface Pixel Format: RGB-565, 16-bit/pixel
  {ST77XX_COLMOD, {0x05}, 1},

  // Column Address Set: 2, 127+2
  {ST77XX_CASET, {0x00, 0x02, 0x00, 0x7F + 0x02}, 4},
  // Row Address Set: 1,159+1
  {ST77XX_RASET, {0x00, 0x01, 0x00, 0x9F + 0x01}, 4},

  // Gamma Adjustments (pos. polarity). Not entirely necessary, but provides accurate colors.
  {ST7735_GMCTRP1,
   {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10},
   16},
  // Gamma Adjustments (neg. polarity). Not entirely necessary, but provides accurate colors.
  {ST7735_GMCTRN1,
   {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10},
   16},
  // Normal Display Mode On
  {ST77XX_NORON, {0}, ST_CMD_DELAY},
  // Display On
  {ST77XX_DISPON, {0}, ST_CMD_DELAY},
  {0, {0}, 0xFF},
};

const lcd_init_cmd_t st7735_display_direc_cmds[] = 
{
  // Memory Data Access Control: top-bottom/left-right refresh
  {ST77XX_MADCTL, {0xB8}, 1},
};

esp_err_t _lcd_spi_send(spi_device_handle_t spi, spi_transaction_t* t)
{
  xSemaphoreTake(_spi_mux, portMAX_DELAY);
  esp_err_t res = spi_device_polling_transmit(spi, t);//Transmit!
  xSemaphoreGive(_spi_mux);
  return res;
}
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
  lcd_dc_t *dc = (lcd_dc_t *) t->user;
  aw_digitalWrite(dc->dc_io, dc->dc_level);
}

uint32_t lcd_spi_init(lcd_conf_t* lcd_conf, spi_device_handle_t *spi_wr_dev, lcd_dc_t *dc, int dma_chan)
{
    if (_spi_mux == NULL) {
        _spi_mux = xSemaphoreCreateMutex();
    }

    if (lcd_conf->init_spi_bus) {
        //Initialize SPI Bus for LCD
        spi_bus_config_t buscfg = {
            .miso_io_num = lcd_conf->pin_num_miso,
            .mosi_io_num = lcd_conf->pin_num_mosi,
            .sclk_io_num = lcd_conf->pin_num_clk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
        };
        spi_bus_initialize(lcd_conf->spi_host, &buscfg, dma_chan);
    }

    spi_device_interface_config_t devcfg = {
        // Use low speed to read ID.
        .clock_speed_hz = 1 * 1000 * 1000,        //Clock out frequency
        .mode = 0,                                //SPI mode 0
        .spics_io_num = lcd_conf->pin_num_cs,     //CS pin
        .queue_size = 7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb = lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };

    // Use high speed to write LCD
    devcfg.clock_speed_hz = lcd_conf->clk_freq;
    //devcfg.flags = SPI_DEVICE_HALFDUPLEX;
    spi_bus_add_device(lcd_conf->spi_host, &devcfg, spi_wr_dev);

    return ESP_OK;
}
void lcd_spi_cmd(spi_device_handle_t spi, const uint8_t cmd, lcd_dc_t *dc)
{
    esp_err_t ret;
    dc->dc_level = LCD_CMD_LEV;
    spi_transaction_t t = {
        .length = 8,                    // Command is 8 bits
        .tx_buffer = &cmd,              // The data is the cmd itself
        .user = (void *) dc,            // D/C needs to be set to 0
    };
    ret = _lcd_spi_send(spi, &t);       // Transmit! 

    assert(ret == ESP_OK);              // Should have had no issues.
}

void lcd_spi_send_uint16_r(spi_device_handle_t spi, const uint16_t data, int32_t repeats, lcd_dc_t *dc)
{
    uint32_t i;
    uint32_t word = data << 16 | data;
    uint32_t word_tmp[16];
    spi_transaction_t t;
    dc->dc_level = LCD_DATA_LEV;

    while (repeats > 0) {
        uint16_t bytes_to_transfer = MIN(repeats * sizeof(uint16_t), SPIFIFOSIZE * sizeof(uint32_t));
        for (i = 0; i < (bytes_to_transfer + 3) / 4; i++) {
            word_tmp[i] = word;
        }

        memset(&t, 0, sizeof(t));           //Zero out the transaction
        t.length = bytes_to_transfer * 8;   //Len is in bytes, transaction length is in bits.
        t.tx_buffer = word_tmp;             //Data
        t.user = (void *) dc;               //D/C needs to be set to 1
        _lcd_spi_send(spi, &t);             //Transmit!
        repeats -= bytes_to_transfer / sizeof(uint16_t);
    }
}
void lcd_spi_data(spi_device_handle_t spi, const uint8_t *data, int len, lcd_dc_t *dc)
{
    esp_err_t ret;
    if (len == 0) {
        return;    //no need to send anything
    }
    dc->dc_level = LCD_DATA_LEV;

    spi_transaction_t t = {
        .length = len * 8,              // Len is in bytes, transaction length is in bits.
        .tx_buffer = data,              // Data
        .user = (void *) dc,            // D/C needs to be set to 1
    };
    ret = _lcd_spi_send(spi, &t);       // Transmit!
    assert(ret == ESP_OK);              // Should have had no issues.
}
/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
void lcd_config(lcd_conf_t *lcd_conf);
void _fastSendRep(uint16_t val, int rep_num);
void _fastSendBuf(const uint16_t* buf, int point_num, bool swap);
void transmitCmdData(uint8_t cmd, uint32_t data);
void transmitDatas(uint8_t* data, int length);
void transmitCmd(uint8_t cmd);
void transmitDataRepat(uint16_t data, int32_t repeats);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void lcd_init(void)
{
  if(lcd_info.spi_mux == NULL)
  {
    lcd_info.spi_mux = xSemaphoreCreateRecursiveMutex();
  }

  lcd_config(&lcd_info.lcd_conf);
  
  mb_library_config_t();

  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;   
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = ((uint64_t)1 << CONFIG_LIBRARY_CS_GPIO) ;// GPIO_OUTPUT_PIN_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  gpio_set_level(CONFIG_LIBRARY_CS_GPIO,0);
  library_spi_cmd(spi_wr_library, 0x66);
  library_spi_cmd(spi_wr_library, 0x99);
  gpio_set_level(CONFIG_LIBRARY_CS_GPIO,1);
}
void lcd_write_command(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t databytes);
void lcd_rotate(uint16_t angle)
{ 
  uint8_t data[16];

  if(angle == 0)
  {
    data[0] = 0xA8;
    lcd_write_command(ST77XX_MADCTL, data, 1, 1);
  }
  else if(angle == 90)
  {
    data[0] = 0x08;
    lcd_write_command(ST77XX_MADCTL, data, 1, 1);
  }  
  else if(angle == 180)
  {
    data[0] = 0x68;
    lcd_write_command(ST77XX_MADCTL, data, 1, 1);
  }  
  else if(angle == 270)
  {
    data[0] = 0xC8;
    lcd_write_command(ST77XX_MADCTL, data, 1, 1);
  }
}

void lcd_set_bg_brightness(uint16_t percentage)
{ 
  // not implement
}

void lcd_on(void)
{ 
  aw_pinMode(lcd_info.lcd_conf.pin_num_bckl, AW_GPIO_MODE_OUTPUT);
  aw_digitalWrite(lcd_info.lcd_conf.pin_num_bckl, (lcd_info.lcd_conf.bckl_active_level) & 0x01);
}


void lcd_off(void)
{ 
  aw_pinMode(lcd_info.lcd_conf.pin_num_bckl, AW_GPIO_MODE_OUTPUT);
  aw_digitalWrite(lcd_info.lcd_conf.pin_num_bckl, (lcd_info.lcd_conf.bckl_active_level) & 0x00);
}

void lcd_write_command(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t databytes)
{
  if(len <= 0)
  {
    return;
  }

  lcd_init_cmd_t lcd_cmd;
  lcd_cmd.cmd = cmd;

  memcpy(lcd_cmd.data, data, len & 0x0f);
  lcd_cmd.databytes = databytes; 

  xSemaphoreTakeRecursive(lcd_info.spi_mux, portMAX_DELAY);
  if(lcd_cmd.databytes != 0xff)
  {
    lcd_spi_cmd(lcd_info.spi_wr, lcd_cmd.cmd, &lcd_info.dc);
    lcd_spi_data(lcd_info.spi_wr, lcd_cmd.data, lcd_cmd.databytes & 0x1F, &lcd_info.dc);
    if(lcd_cmd.databytes & 0x80)
    {
      vTaskDelay(100 / portTICK_RATE_MS);
    }
  }
  xSemaphoreGiveRecursive(lcd_info.spi_mux);
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{ 
  xSemaphoreTakeRecursive(lcd_info.spi_mux, portMAX_DELAY);
  transmitCmdData(LCD_CASET, MAKEWORD(x0 >> 8, ((x0 & 0xFF) + LCD_XSTART), x1 >> 8, ((x1 & 0xFF) + LCD_XSTART)));
  transmitCmdData(LCD_PASET, MAKEWORD(y0 >> 8, ((y0 & 0xFF) + LCD_YSTART), y1 >> 8, ((y1 & 0xFF) + LCD_YSTART)));
  transmitCmd(LCD_RAMWR); // write to RAM
  xSemaphoreGiveRecursive(lcd_info.spi_mux);
}
void lcd_start()
{
  xSemaphoreTakeRecursive(lcd_info.spi_mux, portMAX_DELAY);
}
void lcd_stop()
{
  xSemaphoreGiveRecursive(lcd_info.spi_mux);
}
void lcd_draw(const uint16_t *bitmap, int16_t w, int16_t h)
{
  
  lcd_start();
  setAddrWindow(0, 0, w-1, h-1);
  _fastSendBuf(bitmap,w*h,false);
  lcd_stop();
}
uint16_t color24to16(uint32_t color888)
{
  uint16_t r = (color888 >> 8) & 0xF800;
  uint16_t g = (color888 >> 5) & 0x07E0;
  uint16_t b = (color888 >> 3) & 0x001F;

  return (r | g | b);
}
/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
void lcd_config(lcd_conf_t *lcd_conf)
{
	lcd_info.dc.dc_io = lcd_conf->pin_num_dc;
	// Initialize non-SPI GPIOs
	aw_pinMode(lcd_conf->pin_num_dc, AW_GPIO_MODE_OUTPUT);
	aw_pinMode(lcd_conf->pin_num_rst, AW_GPIO_MODE_OUTPUT);
	aw_digitalWrite(lcd_conf->pin_num_rst, (lcd_conf->rst_active_level) & 0x1);
	vTaskDelay(100 / portTICK_RATE_MS);
	aw_digitalWrite(lcd_conf->pin_num_rst, (~(lcd_conf->rst_active_level)) & 0x1);
	vTaskDelay(100 / portTICK_RATE_MS);

	lcd_spi_init(lcd_conf, &lcd_info.spi_wr, &lcd_info.dc, lcd_info.dma_chan);

	// Send all the init commands of stt35
	int cmd_id = 0;
	const lcd_init_cmd_t* lcd_init_cmds = NULL;
	lcd_init_cmds = st7735_init_cmds;
	assert(lcd_init_cmds != NULL);
	while (lcd_init_cmds[cmd_id].databytes != 0xff)
	{
		lcd_spi_cmd(lcd_info.spi_wr, lcd_init_cmds[cmd_id].cmd, &lcd_info.dc);
		lcd_spi_data(lcd_info.spi_wr, lcd_init_cmds[cmd_id].data, lcd_init_cmds[cmd_id].databytes & 0x1F, &lcd_info.dc);
		if(lcd_init_cmds[cmd_id].databytes & 0x80)
		{
		vTaskDelay(100 / portTICK_RATE_MS);
		}
		cmd_id++;
	}

	aw_pinMode(lcd_conf->pin_num_bckl, AW_GPIO_MODE_OUTPUT);
	aw_digitalWrite(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
}

void _fastSendRep(uint16_t val, int rep_num)
{
  int point_num = rep_num;
  int gap_point = lcd_info.dma_buf_size;
  gap_point = (gap_point > point_num ? point_num : gap_point);

  uint16_t* data_buf = (uint16_t*) malloc(gap_point * sizeof(uint16_t));
  int offset = 0;
  while(point_num > 0)
  {
    for (int i = 0; i < gap_point; i++)
    {
      data_buf[i] = val;
    }

    int trans_points = point_num > gap_point ? gap_point : point_num;
    transmitDatas((uint8_t*) (data_buf), sizeof(uint16_t) * trans_points);
    offset += trans_points;
    point_num -= trans_points;
  }
  free(data_buf);
  data_buf = NULL;
}

void _fastSendBuf(const uint16_t* buf, int point_num, bool swap)
{
  
  if((point_num * sizeof(uint16_t)) <= (16 * sizeof(uint32_t)))
  {
    transmitDatas((uint8_t*) buf, sizeof(uint16_t) * point_num);
  } 
  else
  {
    int gap_point = lcd_info.dma_buf_size;
    int offset = 0;
    while(point_num > 0)
    {
      int trans_points = point_num > gap_point ? gap_point : point_num;
      transmitDatas((uint8_t*) (buf + offset), trans_points * sizeof(uint16_t));
      offset += trans_points;
      point_num -= trans_points;
    }
  }
}

void transmitCmdData(uint8_t cmd, uint32_t data)
{
  xSemaphoreTakeRecursive(lcd_info.spi_mux, portMAX_DELAY);
  lcd_spi_cmd(lcd_info.spi_wr, cmd, &lcd_info.dc);
  lcd_spi_data(lcd_info.spi_wr, (uint8_t *)&data, 4, &lcd_info.dc);
  xSemaphoreGiveRecursive(lcd_info.spi_mux);
}

void transmitDatas(uint8_t* data, int length)
{
  xSemaphoreTakeRecursive(lcd_info.spi_mux, portMAX_DELAY);
  lcd_spi_data(lcd_info.spi_wr, (uint8_t *)data, length, &lcd_info.dc);
  xSemaphoreGiveRecursive(lcd_info.spi_mux);
}

void transmitCmd(uint8_t cmd)
{
  xSemaphoreTakeRecursive(lcd_info.spi_mux, portMAX_DELAY);
  lcd_spi_cmd(lcd_info.spi_wr, cmd, &lcd_info.dc);
  xSemaphoreGiveRecursive(lcd_info.spi_mux);
}

void transmitDataRepat(uint16_t data, int32_t repeats)
{
  xSemaphoreTakeRecursive(lcd_info.spi_mux, portMAX_DELAY);
  lcd_spi_send_uint16_r(lcd_info.spi_wr, data, repeats, &lcd_info.dc);
  xSemaphoreGiveRecursive(lcd_info.spi_mux);
}
