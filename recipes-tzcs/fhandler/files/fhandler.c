#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define BL_DOWN "\
bl_dev=/sys/class/backlight/intel_backlight\n\
max_bl=$(cat $bl_dev/max_brightness)\n\
step=$(($max_bl/8))\n\
temp=$(($step*7))\n\
bn=$(($(cat $bl_dev/brightness) - $step))\n\
if [ $bn -gt $temp ]; then\n\
echo $temp > $bl_dev/brightness\n\
else\n\
[ $bn -lt $step ] || echo $bn > $bl_dev/brightness\n\
fi\n\
"

#define BL_UP "\
bl_dev=/sys/class/backlight/intel_backlight\n\
max_bl=$(cat $bl_dev/max_brightness)\n\
step=$(($max_bl/8))\n\
temp=$(($step*7))\n\
bn=$(($(cat $bl_dev/brightness) + $step))\n\
if [ $bn -gt $temp ]; then\n\
echo $max_bl > $bl_dev/brightness\n\
else\n\
echo $bn > $bl_dev/brightness\n\
fi\n\
"

#define TP_TOGGLE "\
TouchPad=$(grep  -A6 'PS/2 Generic Mouse' /proc/bus/input/devices | grep -Eo 'mouse[0-9]+')\n\
ID=$(xinput list | grep -Eo \"$TouchPad\\s*id\\=[0-9]{1,2}\" | grep -Eo '[0-9]{1,2}' | tail -n1)\n\
STATE=$(xinput list-props $ID | grep 'Device Enabled' | awk '{print $4}')\n\
if [ $STATE -eq 1 ];then\n\
xinput set-prop $ID \"Device Enabled\" 0\n\
else\n\
xinput set-prop $ID \"Device Enabled\" 1\n\
fi\n\
"

#define VL_UP "\
volume=$(amixer sget Master | awk 'END{print $4}' | sed \"s/\\[\\(.*\\)\%\\]/\\1/\")\n\
[ $volume -eq 100 ] || amixer set Master 1%+\n\
"

#define VL_DOWN "\
volume=$(amixer sget Master | awk 'END{print $4}' | sed \"s/\\[\\(.*\\)\%\\]/\\1/\")\n\
[ $volume -le 0 ] || amixer set Master 1%-\n\
"

#define EX_GUI "\
status=$(cat /etc/X11/display.conf)\n\
mon_num=$(xrandr -q | grep \" connected\" | wc -l)\n\
int_mon=$(xrandr -q | grep \"connected\" | awk '{ if (NR == 1) {print $1}}')\n\
ex_mon=$(xrandr -q | grep \" connected\" | awk '{print $1}' | sed -e '1,1d')\n\
ex_mon_off=$(echo $ex_mon | sed \"s/ / --off --output /g\" | sed \"s/$/ --off/\" | sed \"s/^/ --output /\")\n\
ex_mon_on=$(echo $ex_mon | sed \"s/ / --auto --output /g\" | sed \"s/$/ --auto/\" | sed \"s/^/ --output /\")\n\
int_mon_off=$(echo $int_mon | sed \"s/$/ --off/\" | sed \"s/^/ --output /\")\n\
int_mon_on=$(echo $int_mon | sed \"s/$/ --auto/\" | sed \"s/^/ --output /\")\n\
extend=$(echo $ex_mon | sed \"s/ / --auto --right-of $int_mon --output /g\" | sed \"s/$/ --auto --right-of $int_mon/\" | sed \"s/^/ --output /\")\n\
if [ -z \"$ex_mon\" ]; then\n\
	ex_mon_off=$ex_mon\n\
	ex_mon_on=$ex_mon\n\
	extend=$ex_mon\n\
fi\n\
case $status in\n\
        1)\n\
                xrandr $int_mon_on $ex_mon_off\n\
                ;;\n\
        2)\n\
                xrandr $int_mon_off $ex_mon_on\n\
                ;;\n\
        3)\n\
                xrandr $int_mon_on $ex_mon_on\n\
                ;;\n\
        4)\n\
                xrandr $int_mon_on $extend\n\
                ;;\n\
esac\n\
status=$(($status + 1))\n\
[ $status -gt 4 ] && status=1\n\
echo $status > /etc/X11/display.conf\n\
"
char *get_status(const char *cmd);
char *trim(char *str);
const char *getinput = "grep  -E 'Handlers|EV=' /proc/bus/input/devices | grep -B1 'EV=120013' | grep -Eo 'event[0-9]+' | tail -n 1";

/*
 * Cut space of char string
 */

char *trim(char *str) {
        char *end;
        while(isspace(*str)==true) str++;
        if(*str == 0)
                return str;
        end =str +strlen(str)-1;
        while(end > str && isspace(*end)) end--;
        *(end+1) = 0;
        return str;
}

/*
 * Get status of of battery, sound volume, brightness, wifi and touch pad status and can use to get pointer location 
 */

char *get_status(const char *cmd) {
        FILE *fp;
        char path[100];
        char *temp;
        /* Open the command for reading. */
        fp = popen(cmd, "r");
        if (fp == NULL) {
                printf("Failed to run command\n");
        }
        /* Read the output a line at a time - output it. */
        while (fgets(path, sizeof (path) - 1, fp) != NULL) {
                temp = path;
        }
        /* close */
        pclose(fp);
        return strdup(temp);
}


static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
};

int main(void)
{
    char dev[50] = "/dev/input/";
    strcat(dev,get_status(getinput));
    trim(dev);
    struct input_event ev;
    ssize_t n;
    int fd;
    FILE *file;
    size_t charRead;
    int step = 109;

    fd = open(dev, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Cannot open %s: %s.\n", dev, strerror(errno));
        return EXIT_FAILURE;
    }
    while (1) {
        n = read(fd, &ev, sizeof ev);
        if (n == (ssize_t)-1) {
            if (errno == EINTR)
                continue;
            else
                break;
        } else
        if (n != sizeof ev) {
            errno = EIO;
            break;
        }
        if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
        {
            if ((ev.code == KEY_F3 || ev.code == KEY_F4 || ev.code == KEY_F5 || ev.code == KEY_F6 || ev.code == KEY_F7 || ev.code == KEY_F9) && ((strcmp(evval[ev.value], evval[1]) == 0) || strcmp(evval[ev.value], evval[2]) == 0))
            {
                switch (ev.code) {
		    case KEY_F3:
			system(VL_DOWN);
			break;
		    case KEY_F4:
			system(VL_UP);
			break;
                    case KEY_F5:
                        system(EX_GUI);
                        break;
                    case KEY_F6:
                        system(BL_DOWN);
                        break;
                    case KEY_F7:
                        system(BL_UP);
                        break;
                    case KEY_F9:
                        sleep(0.5);
                        system(TP_TOGGLE);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    fflush(stdout);
    fprintf(stderr, "%s.\n", strerror(errno));
    return EXIT_FAILURE;
}
