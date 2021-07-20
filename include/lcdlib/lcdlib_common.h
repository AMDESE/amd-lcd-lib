
#ifndef INCLUDE_LCDLIB_COMMON_H_
#define INCLUDE_LCDLIB_COMMON_H_

#define FILEPATHSIZE 64 
/*  i2c device addr */
#define LCD_MUX_ADDR    0x70    // 0x70 is 7-bit MUX address 
#define LCD_DEV_ADDR    0x28    // 0x28 is 7-bit LCD DEV address

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

/* LCD Message type */
typedef enum
{
	POST_CODE = 1,
	FPGA_ERR,
	BMC_VER,
	BMC_IPADDR
}LCD_msgType_t;

static int  lcdlib_clearScreen(void);
static int  lcdlib_setCursor(int line, int column);
static int  lcdlib_setCursorHome(void);

int lcdlib_open_dev(int i2c_channel);
int lcdlib_close_dev(void);
int lcdlib_write_string(LCD_msgType_t msgType, unsigned char *buffer, int str_len);

#endif  // INCLUDE_LCDLIB_COMMON_H_
