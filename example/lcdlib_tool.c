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
#define LCD_FILE "/var/log/lcd.conf"
#define PC_FILE  "/var/log/post_code"
#define PC_FILE_PATH  "/var/lib/phosphor-post-code-manager/host0/"
#define PC_INDEX      "/var/lib/phosphor-post-code-manager/host0/CurrentBootCycleIndex"
#define LCD_I2C_BUS 1
#define LCD_MAX_LINE 4
#define LCD_MAX_CHAR 20

// LCD Line FW_VER
#define BMC_VER_INDEX    2
#define BIOS_VER_INDEX   4
#define HPM_VER_INDEX    5
#define SCM_VER_INDEX    6

//#define LCD_DEBUG 1
#ifdef LCD_DEBUG
#define print_debug(fmt, args...) printf(fmt, ## args);
#else
#define print_debug(fmt, args...)
#endif

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

static void system_check(char * tmp)
{
    if (system(tmp) < 0 )
        printf("system call failed for %s\n", tmp);
}

/**
Main program.
@param argc number of command line parameters
@param argv list of command line parameters
*/
int main(int argc, char **argv)
{
    FILE *fp;
    char line[LCD_MAX_LINE+3][40];
    char tmp1[20], tmp2[200];
    int i=0;
    int line_fw_ver_index = BMC_VER_INDEX;
    int size  = 0;
    int pc_index = 1;
    int ret;
    union {
      unsigned int  postcode;
      unsigned char bytes[4];
    } pc;
    LCD_msgType_t msgType;

    if (getuid() !=0) {
        rerun_sudo(argc, argv);
    }

    /* create LCD Config file */
    sprintf(tmp2, "rm %s", LCD_FILE);
    system_check(tmp2);
    sprintf(tmp2, "ipmitool lan print |grep \"IP Address  \"| cut -c 27-46 >> %s ", LCD_FILE);
    print_debug("cmd line %s\n", tmp2);
    system_check(tmp2);
    sprintf(tmp2, "cat /etc/os-release |grep \"VERSION_ID\"| cut -c 12-31 >> %s ", LCD_FILE);
    print_debug("cmd line %s\n", tmp2);
    system_check(tmp2);
    sprintf(tmp2, "ipmitool mc  getsysinfo system_name >> %s ", LCD_FILE);
    print_debug("cmd line %s\n", tmp2);
    system_check(tmp2);
    sprintf(tmp2, "busctl get-property xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/bios_active xyz.openbmc_project.Software.Version Version |grep s| cut -c 3-22 >> %s ", LCD_FILE);
    print_debug("cmd line %s\n", tmp2);
    system_check(tmp2);
    sprintf(tmp2, "busctl get-property xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/hpm_fpga_active xyz.openbmc_project.Software.Version Version |grep s| cut -c 3-22 >> %s ", LCD_FILE);
    print_debug("cmd line %s\n", tmp2);
    system_check(tmp2);
    sprintf(tmp2, "busctl get-property xyz.openbmc_project.Software.BMC.Updater /xyz/openbmc_project/software/scm_fpga_active xyz.openbmc_project.Software.Version Version |grep s| cut -c 3-22 >> %s ", LCD_FILE);
    print_debug("cmd line %s\n", tmp2);
    system_check(tmp2);

    /* Open LCD config file */
    fp = fopen(LCD_FILE, "r");
    if (fp == NULL) {
        printf("LCD Error: need LCD file %s\n", LCD_FILE);
	return(-1);
    }

    /* Initialize i2c device */
    if (lcdlib_open_dev(LCD_I2C_BUS) != 0 ) {
       printf("LCD Error: Cannot open LCD I2C Bus %d\n", LCD_I2C_BUS);
    }

    /* Read LCD Config file */
    for (i=1; i < LCD_MAX_LINE+3; i++) {
	strcpy(tmp1, "           ");
	ret = fscanf( fp, "%s", tmp1);
	if (ret == EOF)
	    break;
	switch(i) {
	case 1:
	    sprintf(line[i], "IP: %s ", tmp1);
	    break;
        case 2:
            sprintf(line[i], "BMC: %12s ", tmp1);
            break;
        case 3:
            sprintf(line[i], "HOST: %s ", tmp1);
            break;
        case 4:
            sprintf(line[i], "BIOS: %12s ", tmp1);
            break;
        case 5:
            sprintf(line[i], "HPM_FPGA: %9s ", tmp1);
            break;
        case 6:
            sprintf(line[i], "SCM_FPGA: %9s ", tmp1);
            break;
        default:
	    break;
	}
    }
    fclose(fp);

    // write to LCD, 1st line is POST Code
    for(i=1; i < LCD_MAX_LINE; i++) {
	sleep(1);
	size = strlen(line[i]);
	if(size > LCD_MAX_CHAR)
	    size = LCD_MAX_CHAR;
	else if (size == 00) {
	    strcpy(line[i], "No Data");
	    size = 7;
	}

	msgType = i+1;
	if ( lcdlib_write_string(msgType, line[i], size) !=0 ) {
            printf("LCD Error : writting Line %d\n", msgType);
	    return (-1);
        }
	print_debug("LCD Line %d: %s \n", msgType, line[i]);
    }

    // write the POSt Code
    pc.postcode = 0;
    while(1)
    {
	sleep(2);
        //alternate Line FW_VER
        if (line_fw_ver_index == BMC_VER_INDEX)
                line_fw_ver_index = BIOS_VER_INDEX;
        else if ((line_fw_ver_index == BIOS_VER_INDEX) || 
		 (line_fw_ver_index == HPM_VER_INDEX))
                line_fw_ver_index++;
        else
                line_fw_ver_index = BMC_VER_INDEX;
        size = strlen(line[line_fw_ver_index]);
        if(size > LCD_MAX_CHAR)
            size = LCD_MAX_CHAR;
        else if (size == 00) {
            strcpy(line[i], "No Data");
            size = 7;
        }
	msgType = FW_VER;
        if ( lcdlib_write_string(msgType, line[line_fw_ver_index], size) !=0 ) {
            printf("LCD Error : writting Line %d: %20s\n", msgType, line[line_fw_ver_index]);
            return (-1);
        }
	print_debug("LCD Line %d: %20s \n", msgType, line[line_fw_ver_index]);
	// get the POST Code
	fp =  fopen(PC_INDEX, "r");
	if(fp == NULL)
	    continue;
	ret = fscanf(fp, "%s", tmp2);
	if(ret == EOF ) {
	    fclose(fp);
	    continue;
	}
	while(strstr(tmp2, "value0") == 0){
	    ret = fscanf(fp, "%s", tmp2);
	    if(ret == EOF)
		break;
	}
	ret = fscanf(fp, "%s", tmp1);
	if(ret == EOF ) {
            fclose(fp);
            continue;
        }
	fclose(fp);
	print_debug("PC Index %s\n", tmp1);
	sprintf(tmp2, "%s%s", PC_FILE_PATH, tmp1);
	fp = fopen(tmp2, "r");
	if(fp == NULL)
            continue;
	sprintf(tmp2, "rm %s", PC_FILE);
	system_check(tmp2);
	sprintf(tmp2, "tail %s%s |grep \"tuple_element0\"|cut -c 34-44 >> %s ", PC_FILE_PATH, tmp1, PC_FILE);
	print_debug("cmd line %s\n", tmp2);
	system_check(tmp2);
	fp =  fopen(PC_FILE, "r");
        if(fp == NULL)
            continue;
	fscanf(fp, "%u", &pc.postcode);
	fclose(fp);
	print_debug("PC = %u %02x %02x %02x %02x \n", pc.postcode, pc.bytes[3], pc.bytes[2], pc.bytes[1], pc.bytes[0]);
	sprintf(line[0], "POSTCODE: %02x%02x%02x%02x ", pc.bytes[3], pc.bytes[2], pc.bytes[1], pc.bytes[0]);
	msgType = POST_CODE;
        if ( lcdlib_write_string(msgType, line[0], 18) !=0 ) {
            printf("LCD Error : writting POST Code\n");
	    return (-1);
        }
	print_debug("LCD Line %d: %20s \n", msgType, line[0]);
    }

    //Close the device and exit
    if  (lcdlib_close_dev() != 0 ) {
        printf("LCD Error : failed to close LCD device\n");
        return (-1);
    }
    /* Normal program termination */
    return(0);

}
