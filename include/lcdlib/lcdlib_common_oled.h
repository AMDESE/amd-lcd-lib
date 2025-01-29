#ifdef __cplusplus
extern "C"
{
#endif
#ifndef INCLUDE_LCDLIB_COMMON_OLED_H_
#define INCLUDE_LCDLIB_COMMON_OLED_H_

/* Return Code */
#define LCD_SUCCESS 0x00
#define LCD_ERR_OPEN 0x80
#define LCD_ERR_OPEN_I2C 0x81
#define LCD_ERR_SET_CURSOR 0x82
#define LCD_ERR_BAD_PARAM 0x83
#define LCD_ERR_CLEAR_SCREEN 0x84
#define LCD_ERR_WRITE 0x85
#define LCD_ERR_IOCTL 0x86
#define LCD_ERR_READ 0x87
#define LCD_ERR_ALLOCATE 0x88
#define LCD_ERR_FILED (-1)

#define FILEPATHSIZE 9

#define LINE_HEIGHT_PX 9
#define CHARACTER_WIDTH_PX 6
#define CHARACTER_HEIGHT_PX 8

#define OLED_HEIGHT_PX 64
#define OLED_WIDTH_PX 128
#define OLED_HEIGHT_BYTES 64
#define OLED_WIDTH_BYTES 64

    /* LCD Message type */
    typedef enum
    {
        POST_CODE = 1,
        BMC_IPADDR,
        BMC_VER,
        BIOS_VER,
        HPM_FPGA
    } LCD_msgType_t;

    int lcdlib_write_string(int devId, LCD_msgType_t msgType, unsigned char *buffer, int str_len);
    int lcdlib_clearScreen(int devId);

    extern const unsigned char font6x8_ascii[OLED_WIDTH_PX][CHARACTER_WIDTH_PX];

#endif // INCLUDE_LCDLIB_COMMON_OLED_H_
#ifdef __cplusplus
}
#endif
