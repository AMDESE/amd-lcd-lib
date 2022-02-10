#ifdef __cplusplus
 extern "C" {
#endif
#ifndef INCLUDE_LCDLIB_COMMON_H_
#define INCLUDE_LCDLIB_COMMON_H_

#define FILEPATHSIZE 64 
/*  i2c device addr */
#define LCD_I2C_BUS     1       // LCD virtual i2c bus
#define LCD_MUX_ADDR    0x70    // 0x70 is 7-bit MUX address 
#define LCD_DEV_ADDR    0x28    // 0x28 is 7-bit LCD DEV address

/* MUX commands */
#define MUX_REG         0x00
#define MUX_ENABLE_LCD  0x01    // LCD is on the 1st Port in Genoa

/* LCD commands */
#define CMD_PREFIX      0xFE
#define CMD_CUR_HOME    0x46    // set cursor at home
#define CMD_CUR_SET     0x45    // set cursor at perticular line & column
#define CMD_CLR_SCREEN  0x51    // clear the screen

/* Line code  */
#define LINE1   0x00
#define LINE2   0x40
#define LINE3   0x14
#define LINE4   0x54

/* misc */
#define LCD_MAX_CHAR  20
#define LCD_MAX       200

/* Error Code */
#define LCD_ERR_OPEN         0x80
#define LCD_ERR_OPEN_I2C     0x81
#define LCD_ERR_SET_CURSOR   0x82
#define LCD_ERR_BAD_PARAM    0x83
#define LCD_ERR_CLEAR_SCREEN 0x84
#define LCD_ERR_WRITE        0x85
#define LCD_ERR_IOCTL        0x86

/* LCD Message type */
typedef enum
{
	POST_CODE = 1,
	BMC_IPADDR,
	BMC_VER,
	BIOS_VER
}LCD_msgType_t;

static int  lcdlib_setCursor(int line, int column);
static int  lcdlib_setCursorHome(void);

int lcdlib_open_dev(void);
int lcdlib_close_dev(void);
int lcdlib_write_string(LCD_msgType_t msgType, unsigned char *buffer, int str_len);
int lcdlib_clearScreen(void);

#endif  // INCLUDE_LCDLIB_COMMON_H_
#ifdef __cplusplus
}
#endif
