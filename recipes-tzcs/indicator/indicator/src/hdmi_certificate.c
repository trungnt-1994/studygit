#include <stdio.h>
#include "tvalzctl.h" // tvalz library
#include <fcntl.h>    // lib for open() function
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "hdmi_certificate.h"
#include "helper.h"
#define SCREEN_SIZE_1 "640x480"
#define SCREEN_SIZE_2 "1280x720"
#define SCREEN_SIZE_3 "1920x1080"

int pressed_H;
int position = 1;
int first_call = 1;
char *active_device;
const char *x_command = "xrandr | grep -w connected | cut -d ' ' -f1 | grep -v e | grep HDMI";


/**-------------------------------------------------------------------
 * <B>Function</B>:get_key_pressed_BIOS
 * 
 * Check event press H key when starting BIOS
 *
 * @param unsigned int key
 * @n
 *------------------------------------------------------------------*/
void get_key_pressed_BIOS(unsigned int key)// Check event press H key when starting BIOS
{
    first_call = 1;
    int fd;
    int result;
    fd = open(TVALZ0_FILE_SYSTEM, O_RDONLY);
    if (fd > -1)
    {
        ghci_interface ghci_buf;
        ghci_buf.ghci_eax = 0xFE00;
        ghci_buf.ghci_ebx = 0x00D0;
        ghci_buf.ghci_ecx = 0x0022;
        ghci_buf.ghci_edx = 0x0000;
        ghci_buf.ghci_esi = 0x0000;
        ghci_buf.ghci_edi = 0x0000;
        result = ioctl(fd, IOCTL_TVALZ_GHCI, &ghci_buf);
        if (result == OK){
            if ((ghci_buf.ghci_eax & 0xFF00) != 0){
                pressed_H = 0;
            } else{
                if (ghci_buf.ghci_edx == key){
                    pressed_H = 1;
                }
            }
        }
        
        close(fd);
    }
}


/**-------------------------------------------------------------------
 * <B>Function</B>:get_status_hdmi
 * 
 * Get run HDMI command status
 *
 * @returns   char *
 *
 * @param const char * cmd
 * @n
 *------------------------------------------------------------------*/
char *get_status_hdmi(const char *cmd)// Get run HDMI command status
{
    if (cmd == NULL)
    {
        return NULL;
    }
    FILE *fp;
    char path[100];
    char *temp;
    /* Open the command for reading. */
    fp = popen(cmd, "r");
    if (fp != NULL)
    {
        /* Read the output a line at a time - output it. */
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
 * <B>Function</B>:detected_fn_space
 * 
 * Check event press Fn&Space to using Debug mode
 *
 * @n
 *------------------------------------------------------------------*/
void detected_fn_space()// Check event press Fn&Space to using Debug mode
{

    active_device = get_status_hdmi(x_command);
    if (active_device == NULL)
    {
        return;
    }
    else
    {
        strtok(active_device, "\n");
    }
    exec_command_line(position);
    position++;
    if (position > 3)
    {
        position = 1;
    }
}

/**-------------------------------------------------------------------
 * <B>Function</B>:exec_command_line
 * 
 * Set screen solution following screen position
 *
 * @param int pos
 * @n
 *------------------------------------------------------------------*/
void exec_command_line(int pos)// Set screen solution following screen position
{
    char command[1000]="";
    sprintf(command, "xrandr --output ");
    sprintf(command, "%s %s ", command, active_device);
    sprintf(command, "%s --mode ", command);
    if (pos == 1)
    {
        sprintf(command, "%s %s", command, SCREEN_SIZE_1);
    }
    else if (pos == 2)
    {
        sprintf(command, "%s %s", command, SCREEN_SIZE_2);
    }
    else if (pos == 3)
    {
        sprintf(command, "%s %s", command, SCREEN_SIZE_3);
    }
    system(command);
}
