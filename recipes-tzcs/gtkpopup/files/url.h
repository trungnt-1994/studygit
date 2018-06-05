#ifndef _url_h
#define _url_h
#define BL_DOWN "\
bl_dev=/sys/class/backlight/intel_backlight\n\
step=109\n\
bn=$(($(cat $bl_dev/brightness) - $step))\n\
[ $bn -le 0 ] || echo $bn > $bl_dev/brightness\n\
"

#define BL_UP "\
bl_dev=/sys/class/backlight/intel_backlight\n\
step=109\n\
bn=$(($(cat $bl_dev/brightness) + $step))\n\
[ $bn -gt 872 ] || echo $bn > $bl_dev/brightness\n\
"

#define TP_TOGGLE "\
ID=$(xinput list | grep -Eo 'Generic Mouse\\s*id\\=[0-9]{1,2}' | grep -Eo '[0-9]{1,2}')\n\
STATE=$(xinput list-props $ID | grep 'Device Enabled' | awk '{print $4}')\n\
echo $STATE\n\
"
#define TP_TOGGLE_MAIN "\
TouchPad=$(grep  -A6 'PS/2 Generic Mouse' /proc/bus/input/devices | grep -Eo 'mouse[0-9]+')\n\
ID=$(xinput list | grep -Eo \"$TouchPad\\s*id\\=[0-9]{1,2}\" | grep -Eo '[0-9]{1,2}' | tail -n1)\n\
STATE=$(xinput list-props $ID | grep 'Device Enabled' | awk '{print $4}')\n\
echo $STATE\n\
" 
#define TP_TOGGLE_ON "\
TouchPad=$(grep  -A6 'PS/2 Generic Mouse' /proc/bus/input/devices | grep -Eo 'mouse[0-9]+')\n\
ID=$(xinput list | grep -Eo \"$TouchPad\\s*id\\=[0-9]{1,2}\" | grep -Eo '[0-9]{1,2}' | tail -n1)\n\
xinput set-prop $ID \"Device Enabled\" 1\n\
"
#define TP_TOGGLE_OFF "\
TouchPad=$(grep  -A6 'PS/2 Generic Mouse' /proc/bus/input/devices | grep -Eo 'mouse[0-9]+')\n\
ID=$(xinput list | grep -Eo \"$TouchPad\\s*id\\=[0-9]{1,2}\" | grep -Eo '[0-9]{1,2}' | tail -n1)\n\
xinput set-prop $ID \"Device Enabled\" 0\n\
"
#define CHECK_PLUG "\
int_mon=$(xrandr -q | grep \" connected\" | awk '{print $1}' | sed -e '1,1d')\n\
echo $int_mon > /etc/plug_status.conf \n\
"
#define EX_GUI "\
status=$(cat /etc/X11/display.conf)\n\
mon_num=$(xrandr -q | grep \" connected\" | wc -l)\n\
int_mon=$(xrandr -q | grep \" connected\" | awk '{ if (NR == 1) {print $1}}')\n\
ex_mon=$(xrandr -q | grep \" connected\" | awk '{print $1}' | sed -e '1,1d')\n\
ex_resol=$(xrandr -q | grep -A1 \" connected\" | awk '{print $1}' | tail -n1)\n\
ex_mon_off=$(echo $ex_mon | sed \"s/ / --off --output /g\" | sed \"s/$/ --off/\" | sed \"s/^/ --output /\")\n\
ex_mon_on=$(echo $ex_mon | sed \"s/ / --mode $ex_resol --auto --output /g\" | sed \"s/$/ --mode $ex_resol --auto/\" | sed \"s/^/ --output /\")\n\
int_mon_off=$(echo $int_mon | sed \"s/$/ --off/\" | sed \"s/^/ --output /\")\n\
int_mon_on=$(echo $int_mon | sed \"s/$/ --auto/\" | sed \"s/^/ --output /\")\n\
extend=$(echo $ex_mon | sed \"s/ / --mode $ex_resol --auto --right-of $int_mon --output /g\" | sed \"s/$/ --mode $ex_resol --auto --right-of $int_mon/\" | sed \"s/^/ --output /\")\n\
switchmode=$(echo $ex_mon | sed \"s/ / --mode $ex_resol --auto --left-of $int_mon --output /g\" | sed \"s/$/ --mode $ex_resol --auto --left-of $int_mon/\" | sed \"s/^/ --output /\")\n\
if [ -z \"$ex_mon\" ]; then\n\
        ex_mon_off=$ex_mon\n\
        ex_mon_on=$ex_mon\n\
        extend=$ex_mon\n\
fi\n\
case $status in\n\
        2)\n\
                xrandr $int_mon_on $ex_mon_off\n\
                ;;\n\
        3)\n\
                xrandr $int_mon_off $ex_mon_on\n\
                ;;\n\
        4)\n\
                xrandr $int_mon_on $ex_mon_on\n\
                ;;\n\
        5)\n\
                xrandr $int_mon_on $extend\n\
                ;;\n\
        1)\n\
                xrandr $int_mon_on $switchmode\n\
                ;;\n\
esac\n\
"
#define CHANGE_MONITOR_STATUS "\
status=$(cat /etc/X11/display.conf)\n\
[ $? -eq 0 ] && status=$(($status + 1))\n\
[ $status -gt 5 ] && status=1\n\
echo $status > /etc/X11/display.conf\n\
"
#define EX_INTERNAL "\
xrandr `xrandr --profile single`\n\
echo 2 > /etc/X11/display.conf\n\
"
#define EX_EXTERNAL "\
xrandr `xrandr --profile external`\n\
echo 3 > /etc/X11/display.conf\n\
"
#define EX_CLONE "\
xrandr `xrandr --profile clone`\n\
echo 4 > /etc/X11/display.conf\n\
"
#define EX_EXTEND "\
xrandr `xrandr --profile extend`\n\
echo 5 > /etc/X11/display.conf\n\
"
#define EX_SWITCH "\
xrandr `xrandr --profile switch`\n\
echo 1 > /etc/X11/display.conf\n\
"
#define CK_DIS "\
mon_num=$(xrandr -q | grep \" connected\" | wc -l)\n\
if [ $mon_num -eq 1 ]; then\n\
	echo 2 > /etc/X11/display.conf\n\
else\n\
	echo 4 > /etc/X11/display.conf\n\
fi\n\
"

#define VL_UP "\
volume=$(amixer sget Master | awk '{if (NR == 5) {print $3}}')\n\
[ $volume -eq 87 ] || amixer set Master 1%+\n\
"

#define VL_DOWN "\
volume=$(amixer sget Master | awk '{if (NR == 5) {print $3}}')\n\
[ $volume -le 0 ] || amixer set Master 1%-\n\
"
#define main_window_width 579
#define main_window_heigh 121
#define item_window_width 150
#define window_time_out 4000
#define MAX_CONTROLS 10
char *battery_0_url = "/usr/share/indicator/resource/P0_Status/Battery_0.png";
char *battery_25_url = "/usr/share/indicator/resource/P0_Status/Battery_25.png";
char *battery_50_url = "/usr/share/indicator/resource/P0_Status/Battery_50.png";
char *battery_75_url = "/usr/share/indicator/resource/P0_Status/Battery_75.png";
char *battery_100_url = "/usr/share/indicator/resource/P0_Status/Battery_100.png";
char *battery_charge_url = "/usr/share/indicator/resource/P0_Status/AC.png";

char *network_wire_url = "/usr/share/indicator/resource/P0_Status/Wired.png";
char *wireless_0_url = "/usr/share/indicator/resource/P0_Status/wifi_0.png";
char *wireless_1_url = "/usr/share/indicator/resource/P0_Status/wifi_1.png";
char *wireless_2_url = "/usr/share/indicator/resource/P0_Status/wifi_2.png";
char *wireless_3_url = "/usr/share/indicator/resource/P0_Status/wifi_3.png";
char *wireless_4_url = "/usr/share/indicator/resource/P0_Status/wifi_4.png";
char *wired_small = "/usr/share/indicator/resource/P0_Status/Wired_small.png";
char *wired_big = "/usr/share/indicator/resource/P0_Status/Wired_big.png";
char *wifi_small = "/usr/share/indicator/resource/P0_Status/Wi-Fi_small.png";
char *wifi_big = "/usr/share/indicator/resource/P0_Status/Wi-Fi_big.png";

char *volume_25_url = "/usr/share/indicator/resource/P3_Volume/Volume_0.png";
char *volume_50_url = "/usr/share/indicator/resource/P3_Volume/Volume_50.png";
char *volume_75_url = "/usr/share/indicator/resource/P3_Volume/Volume_75.png";
char *volume_100_url = "/usr/share/indicator/resource/P3_Volume/Volume_100.png";

char *brightness_1_url = "/usr/share/indicator/resource/P4_Brightness/B1.png";
char *brightness_2_url = "/usr/share/indicator/resource/P4_Brightness/B2.png";
char *brightness_3_url = "/usr/share/indicator/resource/P4_Brightness/B3.png";
char *brightness_4_url = "/usr/share/indicator/resource/P4_Brightness/B4.png";
char *brightness_5_url = "/usr/share/indicator/resource/P4_Brightness/B5.png";
char *brightness_6_url = "/usr/share/indicator/resource/P4_Brightness/B6.png";
char *brightness_7_url = "/usr/share/indicator/resource/P4_Brightness/B7.png";
char *brightness_8_url = "/usr/share/indicator/resource/P4_Brightness/B8.png";
char *brightness_level_active = "/usr/share/indicator/resource/P4_Brightness/Brightness_active_unit.png";
char *brightness_level_inactive = "/usr/share/indicator/resource/P4_Brightness/Brightness_inactive_unit.png";

char *multi_display_url = "/usr/share/indicator/resource/displayport/Multi_Display.png";

char *touchpad_url = "/usr/share/indicator/resource/touchpad/touchpad.png";
char *touchpad_on_url = "/usr/share/indicator/resource/P5_Touchpad/Touch_ON_small.png";
char *touchpad_on_selected_url = "/usr/share/indicator/resource/P5_Touchpad/Touch_ON_big.png";
char *touchpad_off_url = "/usr/share/indicator/resource/P5_Touchpad/Touch_OFF_small.png";
char *touchpad_off_selected_url = "/usr/share/indicator/resource/P5_Touchpad/Touch_OFF_big.png";

char *clone_display = "/usr/share/indicator/resource/P2_Monitors/Monitor_Clone_85.png";
char *clone_display_hover = "/usr/share/indicator/resource/P2_Monitors/Monitor_Clone_big.png";
char *extend_display = "/usr/share/indicator/resource/P2_Monitors/Monitor_Extend_85.png";
char *extend_display_hover = "/usr/share/indicator/resource/P2_Monitors/Monitor_Extend_big.png";
char *switch_display = "/usr/share/indicator/resource/P2_Monitors/Monitor_Switch_85.png";
char *switch_display_hover = "/usr/share/indicator/resource/P2_Monitors/Monitor_Switch_big.png";
char *internal_display = "/usr/share/indicator/resource/P2_Monitors/Monitor_Internal_85.png";
char *internal_display_hover = "/usr/share/indicator/resource/P2_Monitors/Monitor_Internal_big.png";
char *external_display = "/usr/share/indicator/resource/P2_Monitors/Monitor_External_85.png";
char *external_display_hover = "/usr/share/indicator/resource/P2_Monitors/Monitor_External_big.png";

char *back_icon_url = "/usr/share/indicator/resource/P1_Function/back_icon.png";
char *increase_icon_url = "/usr/share/indicator/resource/P1_Function/increase_icon.png";
char *decrease_icon_url = "/usr/share/indicator/resource/P1_Function/decrease_icon.png";
char *close_icon_url = "/usr/share/indicator/resource/P1_Function/close_icon.png";
char *brightness_up_url = "/usr/share/indicator/resource/P4_Brightness/Brightness_up_icon.png";
char *brightness_down_url = "/usr/share/indicator/resource/P4_Brightness/Brightness_down_icon.png";

char *warning = "/usr/share/indicator/resource/warning/warning.png";
#endif
