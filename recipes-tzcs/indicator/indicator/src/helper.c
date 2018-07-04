#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "helper.h"
#include "tvalzctl.h"
#include <unistd.h>
#include <errno.h>
char *TVALZ0_FILE_SYSTEM        = "/dev/tvalz0";

/**-------------------------------------------------------------------
 * <B>Function</B>:get_status
 * 
 * Get status of run command
 *
 * @returns   char *
 *
 * @param const char * cmd
 * @n
 *------------------------------------------------------------------*/
char *get_status(const char *cmd)// Get status of run command
{
    FILE *fp;
    char path[100];
    char *temp = "";
    fp = popen(cmd, "r");
    if (fp != NULL)
    {
        while (fgets(path, sizeof(path) - 1, fp) != NULL)
        {
            temp = path;
        }
        pclose(fp);
        return strdup(temp);
    }
    else
    {
        printf("Failed to run command #002\n");
        return NULL;
    }
}


/**-------------------------------------------------------------------
 * <B>Function</B>:trim
 * 
 * Cut space of char
 *
 * @returns   char *
 *
 * @param char * str
 * @n
 *------------------------------------------------------------------*/
char *trim(char *str) //Cut space of char
{
    char *end;
    while (isspace(*str))
    {
        str++;
    }

    if (*str == 0)
    {
        return str;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end))
    {
        end--;
    }

    *(end + 1) = 0;
    return str;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:get_info_val
 * 
 * Get indicator of battery, sound volume, brightness, wifi and touch pad status and can use to get pointer location 
 *
 * @returns   int
 *
 * @param const char * cmd
 * @n
 *------------------------------------------------------------------*/
int get_info_val(const char *cmd) /*  Get indicator of battery, sound volume, brightness, 
                                  wifi and touch pad status and can use to get pointer location */
{
    int result;
    FILE *fp;
    char path[100];
    fp = popen(cmd, "r");
    if (fp == NULL)
    {
        printf("Failed to run command #001\n");
        return 0;
    }
    while (fgets(path, sizeof(path) - 1, fp) != NULL)
    {
        result = atoi(path);
    }
    pclose(fp);
    return result;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:init_hardware_driver
 * 
 * Enable some option for tvalz file
 *
 * @returns   int
 *
 * @n
 *------------------------------------------------------------------*/
int init_hardware_driver()// Enable some option for tvalz file
{
    int fd;
    int value = 1;
    printf("Init tvalz\n");
    fd = open(TVALZ0_FILE_SYSTEM, O_RDONLY);
    if (fd < 0)
    {
        perror(TVALZ0_FILE_SYSTEM);
        return EXIT_FAILURE;
    }

    ioctl(fd, IOCTL_TVALZ_ENAB, 0);
    ioctl(fd, IOCTL_TVALZ_ENABLE_INPUTDEV, &value);
    close(fd);
    return EXIT_SUCCESS;
}

// ome-20161219 modify-s
void get_client_status_kill()
{

    /*    system("cat /sys/class/net/eth0/operstate");
    *    system("ps | grep wget");
    *    system("ps | grep GetClientStatus");
    */
    system("for k in `ps | awk '/'ExchData'/{print $1}'`; do kill $k 2>/dev/null; done");
    /*    system("for k in `ps | awk '/'GetClientStatus'/{print $1}'`; do kill $k 2>/dev/null; done");
    *    system("cat /sys/class/net/eth0/operstate");
    *    system("ps | grep wget");
    *    system("ps | grep GetClientStatus");
    */
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    acces_BIOS_value
 * 
 * Get/Set BIOS value
 *
 * @return Return OK in normal case or error code if an error occurred
 *
 * @param int access_mode  (read/write)
 * @param int object_type  (brightness/touch pad)
 * @param ULONG rev_value  (receive value)
 * @param ULONG mod_value  (modifier value)
 * @n
 *------------------------------------------------------------------*/
int acces_BIOS_value(int access_mode, int object_type, ULONG* rev_value, ULONG mod_value)// Get/Set BIOS value 
{
    ghci_interface ghci_buf;
    int fd;
    int return_status = OK;
    fd = open("/dev/tvalz0", O_RDONLY);
    if (fd < 0){
        return_status = errno;
    }else{
        // Init ghci
        ghci_buf.ghci_eax = access_mode;
        ghci_buf.ghci_ebx = object_type;
        ghci_buf.ghci_ecx = mod_value;
        ghci_buf.ghci_edx = 0x00;
        ghci_buf.ghci_esi = 0x00;
        ghci_buf.ghci_edi = 0x00;
        ioctl(fd, IOCTL_TVALZ_GHCI,  &ghci_buf);
        close(fd);
    }
    *rev_value = ghci_buf.ghci_ecx;
    return return_status;
}
