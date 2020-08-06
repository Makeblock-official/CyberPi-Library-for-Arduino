#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../io/aw9523b.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"


#define ST_CMD_DELAY 0x80  // special signifier for command lists

#define ST77XX_NOP 0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID 0x04
#define ST77XX_RDDST 0x09

#define ST77XX_SLPIN 0x10
#define ST77XX_SLPOUT 0x11
#define ST77XX_PTLON 0x12
#define ST77XX_NORON 0x13

#define ST77XX_INVOFF 0x20
#define ST77XX_INVON 0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON 0x29
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_RAMRD 0x2E

#define ST77XX_PTLAR 0x30
#define ST77XX_TEOFF 0x34
#define ST77XX_TEON 0x35
#define ST77XX_MADCTL 0x36
#define ST77XX_COLMOD 0x3A

#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1 0xDA
#define ST77XX_RDID2 0xDB
#define ST77XX_RDID3 0xDC
#define ST77XX_RDID4 0xDD

// ST7735 commands
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH 0x04

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;



/******************************************************************************
 DEFINE MACROS
 ******************************************************************************/
#define TAG                               ("esp32_lcd")

#define LCD_TFTHEIGHT 128
#define LCD_TFTWIDTH 128
#define LCD_XSTART    (3)
#define LCD_YSTART    (2)   

#define LCD_INVOFF    0x20
#define LCD_INVON     0x21

#define LCD_CASET   0x2A
#define LCD_PASET   0x2B
#define LCD_RAMWR   0x2C
#define LCD_MADCTL  0x36

// Color definitions
#define COLOR_BLACK       0x0000      /*   0,   0,   0 */
#define COLOR_NAVY        0x000F      /*   0,   0, 128 */
#define COLOR_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define COLOR_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define COLOR_MAROON      0x7800      /* 128,   0,   0 */
#define COLOR_PURPLE      0x780F      /* 128,   0, 128 */
#define COLOR_OLIVE       0x7BE0      /* 128, 128,   0 */
#define COLOR_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define COLOR_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define COLOR_BLUE        0x001F      /*   0,   0, 255 */
#define COLOR_GREEN       0x07E0      /*   0, 255,   0 */
#define COLOR_CYAN        0x07FF      /*   0, 255, 255 */
#define COLOR_RED         0xF800      /* 255,   0,   0 */
#define COLOR_MAGENTA     0xF81F      /* 255,   0, 255 */
#define COLOR_YELLOW      0xFFE0      /* 255, 255,   0 */
#define COLOR_WHITE       0xFFFF      /* 255, 255, 255 */
#define COLOR_ORANGE      0xFD20      /* 255, 165,   0 */
#define COLOR_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define COLOR_PINK        0xF81F
#define COLOR_SILVER      0xC618
#define COLOR_GRAY        0x8410
#define COLOR_LIME        0x07E0
#define COLOR_TEAL        0x0410
#define COLOR_FUCHSIA     0xF81F
#define COLOR_ESP_BKGD    0xD185

#define LCD_CMD_LEV   (0)
#define LCD_DATA_LEV  (1)

#define LCD_MOSI_GPIO              (2)
#define LCD_MISO_GPIO              (26) // for font library
#define LCD_CLK_GPIO               (4)
#define LCD_CS_GPIO                (12)
#define LCD_DC_GPIO                (AW_P1_4)
#define LCD_RESET_GPIO             (AW_P1_5)
#define LCD_BL_GPIO                (AW_P1_7)
#define CONFIG_LCD_SPI_NUM                (1)
#define CONFIG_LCD_SPI_CLOCK              (20000000)
#define CONFIG_LCD_BCKL_ACTIVE_LEVEL      (1) 

#define CONFIG_LCD_MOSI_GPIO              LCD_MOSI_GPIO
#define CONFIG_LCD_MISO_GPIO              LCD_MISO_GPIO
#define CONFIG_LCD_CLK_GPIO               LCD_CLK_GPIO
#define CONFIG_LCD_CS_GPIO                LCD_CS_GPIO
#define CONFIG_LCD_DC_GPIO                LCD_DC_GPIO
#define CONFIG_LCD_RESET_GPIO             LCD_RESET_GPIO
#define CONFIG_LCD_BL_GPIO                LCD_BL_GPIO
  
#define SPIFIFOSIZE 16
#define SPI_DMA_BUFFER_SIZE               (1024)
#define SPI_DMA_CHANNEL                   (1)

#define SWAPBYTES(i) ((i>>8) | (i<<8))
#define MAKEWORD(b1, b2, b3, b4) ((uint32_t) ((b1) | ((b2) << 8) | ((b3) << 16) | ((b4) << 24)))

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define DEFAULT_BACKGROUND_COLOR (COLOR_BLACK)
/******************************************************************************
 DEFINE TYPES & CONSTANTS
 ******************************************************************************/

typedef struct {
    uint8_t dc_io;
    uint8_t dc_level;
} lcd_dc_t;

typedef enum {
    LCD_MOD_ST7789 = 1,
    LCD_MOD_AUTO_DET = 3,
} lcd_model_t;

/**
 * @brief struct to map GPIO to LCD pins
 */
typedef struct {
    lcd_model_t lcd_model;
    int8_t pin_num_miso;        /*!<MasterIn, SlaveOut pin*/
    int8_t pin_num_mosi;        /*!<MasterOut, SlaveIn pin*/
    int8_t pin_num_clk;         /*!<SPI Clock pin*/
    int8_t pin_num_cs;          /*!<SPI Chip Select Pin*/
    int8_t pin_num_dc;          /*!<Pin to select Data or Command for LCD*/
    int8_t pin_num_rst;         /*!<Pin to hardreset LCD*/
    int8_t pin_num_bckl;        /*!<Pin for adjusting Backlight- can use PWM/DAC too*/
    int clk_freq;                /*!< spi clock frequency */
    uint8_t rst_active_level;    /*!< reset pin active level */
    uint8_t bckl_active_level;   /*!< back-light active level */
    spi_host_device_t spi_host;  /*!< spi host index*/
    bool init_spi_bus;
} lcd_conf_t;

/**
 * @brief struct holding LCD IDs
 */
typedef struct {
    uint8_t mfg_id;         /*!<Manufacturer's ID*/
    uint8_t lcd_driver_id;  /*!<LCD driver Version ID*/
    uint8_t lcd_id;         /*!<LCD Unique ID*/
    uint32_t id;
} lcd_id_t;

typedef struct
{
  uint16_t height;
  uint16_t width;
  uint16_t current_angle;

  uint16_t dma_mode;
  int      dma_buf_size;
  uint16_t  dma_chan;
  spi_device_handle_t spi_wr_handle;
  SemaphoreHandle_t spi_mux;

  spi_device_handle_t spi_wr;
  lcd_conf_t lcd_conf;

  lcd_dc_t dc;
  
}lcd_info_t;

/******************************************************************************
 DECLARE PRIVATE DATAS
 ******************************************************************************/
esp_err_t _lcd_spi_send(spi_device_handle_t spi, spi_transaction_t* t);
void lcd_spi_pre_transfer_callback(spi_transaction_t *t);

uint32_t lcd_spi_init(lcd_conf_t* lcd_conf, spi_device_handle_t *spi_wr_dev, lcd_dc_t *dc, int dma_chan);
void lcd_spi_cmd(spi_device_handle_t spi, const uint8_t cmd, lcd_dc_t *dc);

void lcd_spi_send_uint16_r(spi_device_handle_t spi, const uint16_t data, int32_t repeats, lcd_dc_t *dc);
void lcd_spi_data(spi_device_handle_t spi, const uint8_t *data, int len, lcd_dc_t *dc);
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
void lcd_init(void);
void lcd_write_command(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t databytes);
void lcd_rotate(uint16_t angle);

void lcd_set_bg_brightness(uint16_t percentage);

void lcd_on(void);


void lcd_off(void);

void lcd_write_command(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t databytes);

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcd_start();
void lcd_stop();
void lcd_draw(const uint16_t *bitmap, int16_t w, int16_t h);
/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
void lcd_config(lcd_conf_t *lcd_conf);

void _fastSendRep(uint16_t val, int rep_num);
void _fastSendBuf(const uint16_t* buf, int point_num, bool swap);

void transmitCmdData(uint8_t cmd, uint32_t data);
void transmitDatas(uint8_t* data, int length);

void transmitCmd(uint8_t cmd);
void transmitDataRepat(uint16_t data, int32_t repeats);
uint16_t color24to16(uint32_t color888);

int get_utf8_data(uint32_t letter, uint8_t word_size, uint8_t *map, bool *elongate,uint8_t *width,uint8_t* height);

#endif