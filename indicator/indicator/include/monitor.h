#ifndef _MONITOR_
#define _MONITOR_
#include <X11/extensions/Xrandr.h>

#define CHECK_PLUG "\
	int_mon=$(xrandr -q | grep \" connected\" | sed -e \"1,1d\" | awk '{print $1}' | tr '\n' ' ' | sed -e \"s/.$//g\")\n \
	if [ -f /etc/plug_status.conf ] then sed -i -e \"1s/.*/$int_mon/g\" /etc/plug_status.conf \
	else echo $int_mon > /etc/plug_status.conf fi\n"
#define GET_CURRENT_PROFILE   "xrandr |grep \"Current Profile\" |cut -d ':' -f2"
#define CHECK_PLUG_VGA        "cat /sys/class/drm/card0-DP*/status | grep ^connected\n"
#define CHECK_PLUG_HDMI       "cat /sys/class/drm/card0-HDMI*/status | grep ^connected\n"
#define EX_INTERNAL           "xrandr `xrandr --profile single`\n"
#define EX_EXTERNAL           "xrandr `xrandr --profile external`\n"
#define EX_CLONE              "xrandr `xrandr --profile clone`\n"
#define EX_EXTEND             "xrandr `xrandr --profile extend`\n"
#define EX_SWITCH             "xrandr `xrandr --profile switch`\n"

/** Constant definition*/
/** Enum state definition*/
enum Monitor {
    INTERNAL,
    EXTERNAL,
    CLONE,
    EXTEND,
    SWITCH,/*switch monitor*/
};
/** Enum state definition*/
enum MonitorExtend {
    INTERNAL_SCREEN,
    VGA,
    HDMI
};
/** Enum action definition*/
enum MonitorAction {
    NO_ACTION,
    PLUG_IN,
    UN_PLUG
};
/** Xrandr monitor mode*/
typedef enum _profile {
    profile_single = 1,
    profile_external,
    profile_clone ,
    profile_extend_right,
    profile_switch_right,
    profile_extend_below,
    profile_extend_left,
    profile_extend_above,
    profile_switch_below,
    profile_switch_left,
    profile_switch_above
} profile_t;
/** Structure monitor*/
typedef struct _MonitorStructure{
    enum Monitor            current_mode;   // current monitor mode
    int                     un_plugVGA;     //VGA status
    int                     un_plugHDMI;    //HDMI status
    enum MonitorExtend      current_output; // 0: nothing | 1: VGA | 2: HDMI
    int                     first_init_time;
    int                     is_switching;
    int                     internal_is_pri;
    profile_t               extend_type;
} MonitorInfo, *PointerMonitorInfo;

/** Screen properties **/
typedef struct _OutputScreen{
    char* name;
    int width;
    int height;
} OutputScreen, *pOutputScreen;
/** Outputs structure **/
typedef struct _Outputs{
    pOutputScreen out_screens;
    int nscreen;
    int nprimary;
    int nsecond;
    int ninternal;
    int ncrt_use;
} Outputs, *pOutputs;
/**  Global variable definition*/
extern MonitorInfo monitor_info_s;

/**  Public function */
void init_monitor_status(void);
void update_plugged_status(void);
void set_monitor_mode(int select_mode);
int synchronize_monitor_status();
pOutputs get_screen_info();

#endif

