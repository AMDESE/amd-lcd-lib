#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <lcdlib/lcdlib_common.h>

static int fd = -1;

/*
* Set cursor at line + column position
*/
static int  lcdlib_setCursor(int line, int col)
{
    char data[2];

    if (fd < 0)
        return (-1);

    if (line > 0 && line < 5 && col > -1 && col < 20) {
        data[0] = CMD_CUR_SET;
        switch (line) {
            case 1: data[1] = LINE1 + col; break;
            case 2: data[1] = LINE2 + col; break;
            case 3: data[1] = LINE3 + col; break;
            case 4: data[1] = LINE4 + col; break;
            default : data[1] = LINE1 + 0; break;
        }

        if (i2c_smbus_write_i2c_block_data(fd, CMD_PREFIX, 2, data) != 0) {
            printf("Error: Failed to set cursor position\n");
            return -(-1);
        }
    }
    else {
        printf("Error: line & column parameter out of range, (line [1-4], col [1-20])\n");
        return (-1);
    }

    return (0);
}

/*
* Set cursor at Home position
*/
static int  lcdlib_setCursorHome(void)
{
    if (fd <  0) 
        return (-1);

    /* Set cursor at home position */
    if (i2c_smbus_write_byte_data(fd, CMD_PREFIX, CMD_CUR_HOME) != 0) {
        printf("Error: Failed to set cursor home\n");
        return (-1);
    }

    return (0);
}

/*
 *Clear LCD screen
 *
 */
static int lcdlib_clearScreen(void)
{
    if (fd < 0)
        return (-1);

    /* Clear the LCD screen */
    if (i2c_smbus_write_byte_data(fd, CMD_PREFIX, CMD_CLR_SCREEN) != 0) {
        printf("Error: Failed to clear the screen\n");
        return (-1);
     }

    return (0);
}

/*
 * Write a string on the screen
 */
int  lcdlib_write_string(LCD_msgType_t msgType, unsigned char *buffer, int str_len)
{
    int column = 0;
    if (fd < 0) 
        return (-1);

    if (lcdlib_setCursor((int)msgType, column) !=0)
        return (-1);

    usleep(1000);
    /* Write a string to LCD screen */
    if (i2c_smbus_write_i2c_block_data(fd, buffer[0], (str_len-1), &buffer[1])!=0) {
        printf("Error: Failed to write string on the screen\n");
        return (-1);
    }

    return (0);
}

/*
 * Initialization step, where Opening the i2c device file.
 */
int lcdlib_open_dev(int i2c_bus)
{
    int8_t *buffer;
    int data;
    char i2c_devname[FILEPATHSIZE];


    snprintf(i2c_devname, FILEPATHSIZE, "/dev/i2c-%d", i2c_bus);
    if (fd < 0) {
        fd = open(i2c_devname, O_RDWR);
        if (fd < 0) {
            printf("Error: Failed to open i2c device\n");
            return (-1);
        }

	// Set MUX i2c device address
        if (ioctl(fd, I2C_SLAVE, LCD_MUX_ADDR) < 0) {
            printf("Error: Failed setting Mux i2c dev addr 0x%x\n", LCD_MUX_ADDR);
            return (-1);
        }

        // Enable LCD Bus in Mux
        if (i2c_smbus_write_byte_data(fd, MUX_REG, MUX_ENABLE_LCD) != 0) {
            printf("Error: Failed to enable LCD bus in Mux\n");
            return (-1);
        }

	/* Set LCD i2c device address */
        if (ioctl(fd, I2C_SLAVE, LCD_DEV_ADDR) < 0) {
            printf("Error: Failed setting i2c dev addr\n");
            return (-1);
        }

        /* Clear screen */
        if (lcdlib_clearScreen() !=0 )
            printf("Error: Failed to clear the screen\n");
    }
    else {
        printf("Error: failed to open LCD device\n");
        return (-1);
    }
    usleep(10 * 1000);

    return (0);
}

/*
 * Close the i2c device file in the exit.
 */
int lcdlib_close_dev(void)
{
    if (fd >= 0) {
        close(fd);
    }

    fd = -1;
    return (0);
}
