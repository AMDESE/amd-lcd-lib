#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lcdlib/lcdlib_common.h>

#define ARGS_MAX 64

static void rerun_sudo(int argc, char **argv)
{
        static char *args[ARGS_MAX];
        char sudostr[] = "sudo";
        int i;

        args[0] = sudostr;
        for (i = 0; i < argc; i++) {
                args[i + 1] = argv[i];
        }
        args[i + 1] = NULL;
        execvp("sudo", args);
}

/**
Main program.
@param argc number of command line parameters
@param argv list of command line parameters
*/
int main(int argc, char **argv)
{
    int i2c_bus = 1;
//    unsigned char postcode_str[] = "PostCode: 1D69";
    unsigned char postcode[20];
    unsigned char bmcip[20];
    unsigned char bmcver[20];
    LCD_msgType_t msgType = POST_CODE;

    int line = 1;

    if (getuid() !=0) {
        rerun_sudo(argc, argv);
    }

    /* Initialize i2c device */
    if (lcdlib_open_dev(i2c_bus) == 0 ) {
        /* Write 'Post code' string */
        printf("Printing Post Code...\n");
        for ( int i=0; i < 10; i++ ) {
            sprintf(postcode, "PostCode: %04X", ((rand() % 9000)+ 1000));
            if ( lcdlib_write_string(msgType, postcode, strlen(postcode) ) !=0 ) {
                printf("Error : Failed to display Post Code\n");
            }
            sleep(1);
        }
    } 

    sleep(2);                    
    printf("Printing BMC IP...\n");
    for ( int i=0; i < 10; i++ ) {
        sprintf(bmcip, "IP:19%d.168.245.17%d", i, i);
        msgType = BMC_IPADDR;
        if ( lcdlib_write_string(msgType, bmcip, strlen(bmcip) ) !=0 ) {
            printf("Error : Failed to display IP address\n");
        }
            sleep(1);
    }   
    sleep(2);
    printf("Printing BMC Version ...\n");
    sprintf(bmcver, "BMCver:2.11.00-dev");
    msgType = BMC_VER;
    if ( lcdlib_write_string(msgType, bmcver, strlen(bmcver) ) !=0 ) {
        printf("Error :Failed to display BMC Version\n");
    }

    if  (lcdlib_close_dev() != 0 ) {
        printf("Error : failed to close LCD device\n");
        return (-1);
    }
    /* Normal program termination */
    return(0);
}
