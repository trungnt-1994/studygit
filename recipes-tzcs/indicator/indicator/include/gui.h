#ifndef _WINDOW_
#define _WINDOW_

#include <gtk/gtk.h>
#include <glib.h>
#include "monitor.h"
#include "brightness.h"
#include "network.h"
#include "touchpad.h"
#include "battery.h"
#include "config.h"

//#define DEBUG_GUI                   1

/**  Indicator constant */
#ifdef ENABLE_CTRLPANEL
#define MAIN_WINDOW_WIDTH           714
#else
#define MAIN_WINDOW_WIDTH           571
#endif
#define MAIN_WINDOW_HEIGHT          123
#define CONTENT_FRAME_HEIGHT        86
/**  Animation constant */
#define ANIMATION_STEP              1
#define ANIMATION_UP                2
#define ANIMATION_DOWN              1
#define ANIMATION_HIDE              -1
#define ANIMATION_INTERVAL          2

/**  Home Item */
#define HOME_ITEM_NUMBER            4
enum Home_Item {
                MONITOR_ITEM,
                BRIGHTNESS_ITEM,
                NETWORK_ITEM,
                TOUCHPAD_ITEM
};

/**  Monitor Item */
#define MONITOR_MODE                5
#define MONITOR_ITEM_NUMBER         5
#ifdef ENABLE_CTRLPANEL
#define ITEM_EXTEND_WIDTH           179
#define ITEM_SWITCH_WIDTH           143
#else
#define ITEM_EXTEND_WIDTH           143
#define ITEM_SWITCH_WIDTH           111
#endif

/**  Network item */
#define NETWORK_MODE                2
#define NETWORK_ITEM_NUMBER         2

/**  Touch pad item */
#define TOUCHPAD_ITEM_NUMBER        2

/**  Brightness item */
#define BRIGHTNESS_ITEM_NUMBER      8
enum Brighness_mode{
                ACTIVE,
                INACTIVE
};

/**  Battery status */
#define STATUS_ITEM_NUMBER          5
#define UPDATE_STATUS_INTERVAL      1
enum Status_item {
                BLOCK_STT,
                NETWORK_STT,
                BATTERY_ICON,
                BATTERY_STT,
                CHARGER_STT
};

/** Option to display */
enum Frame_Type{
                HOME_FRAME_TYPE,
                MONITOR_FRAME_TYPE,
                BRIGHTNESS_FRAME_TYPE,
                NETWORK_FRAME_TYPE,
#ifdef ENABLE_CTRLPANEL
                CTRLPANEL_FRAME_TYPE,
#endif
                TOUCHPAD_FRAME_TYPE
};

/** Struct for status frame*/
typedef struct _StatusFrameStruct {
    PointerNetworkInfo      p_network_info;
    PointerBatteryInfo      p_battery_info;
    GtkWidget               *frame;
    GtkWidget               *status_img[STATUS_ITEM_NUMBER];
} StatusFrame, *PointerStatusFrame;

/** Struct for home frame*/
typedef struct _HomeFrame {
    int                     *home_status[HOME_ITEM_NUMBER];
    GtkWidget               *frame;
    GtkWidget               *home_img[HOME_ITEM_NUMBER];
    GtkWidget               *ctrlpanel_box;
} HomeFrame, *PointerHomeFrame;

/**  Struct of monitor frame */
typedef struct _MonitorFrame {
    int                     monitor_select;
    PointerMonitorInfo      p_monitor_info;
    GtkWidget               *frame;
    GtkWidget			    *monitor_frame[MONITOR_ITEM_NUMBER];
    GtkWidget               *monitor_img[MONITOR_ITEM_NUMBER];
} MonitorFrame, *PointerMonitorFrame;

/**  Struct of network frame */
typedef struct _BrightnessFame {
    PointerBrightnessInfo   p_brightness_info;
    GtkWidget               *frame;
    GtkWidget               *brightness_img[BRIGHTNESS_ITEM_NUMBER];
} BrightnessFame, *PointerBrightnessFame;

/**  Struct of network frame */
typedef struct _NetworkFrame {
    int                     network_select;
    PointerNetworkInfo      p_network_info;
    GtkWidget               *frame;
    GtkWidget               *network_img[NETWORK_ITEM_NUMBER];
} NetworkFrame,*PointerNetworkFrame;

/**  Struct of touchpad frame */
typedef struct _TouchPadFrame {
    int                     mode_select;
    PointerTouchPadInfo     p_touchpad_info;
    GtkWidget               *frame;
    GtkWidget               *touchpad_img[TOUCHPAD_ITEM_NUMBER];
} TouchPadFrame, *PointerTouchPadFrame;

#ifdef ENABLE_CTRLPANEL
/**  Struct of touchpad frame */
typedef struct _CtrlPanelFrame {
    GtkWidget               *frame;
} CtrlPanelFrame, *PointerCtrlPanelFrame;
#endif

/**  Struct of animation data */
typedef struct _WindowData {
    int                     w, h;
    int                     x,y;
    int                     direction;
    guint                   timer_show_id;
    int                     is_animation;
    int                     is_showing;
} WindowData;
/**  Struct of GUI manager */
typedef struct _GUIManager{
    PointerStatusFrame      p_fr_status;
    PointerHomeFrame        p_fr_home;
    PointerMonitorFrame     p_fr_monitor;
    PointerBrightnessFame   p_fr_bright;
    PointerNetworkFrame     p_fr_network;
    PointerTouchPadFrame     p_fr_touchpad;
#ifdef ENABLE_CTRLPANEL
    PointerCtrlPanelFrame    p_fr_ctrlpanel;
#endif
    GtkWidget               *p_current_fr;
    int                     current_frame;
    int                     screen1_width;
    int                     screen2_width;
    WindowData              st_animationdata;
    GtkWidget               *window;
} GUIManager, *PointerGUIManager;



/**  FUNCTION DEFINITION*/
GtkWidget * init_frame(void);
void update_window_data(void);
void show_frame(int item);
gboolean show_animation(void *dataset);
void update_extend_monitor(PointerMonitorFrame p_monitor);
void change_frame_network_icon(PointerNetworkFrame p_network);
void change_frame_brightness_icon(PointerBrightnessFame p_brightness);
void change_frame_monitor_icon(PointerMonitorFrame p_monitor);
void change_frame_touchpad_icon(PointerTouchPadFrame p_touchpad);
void update_status_icon(PointerStatusFrame p_status);
void update_home_frame(void);
void update_network_icon(void);
void update_touchpad_icon(void);
/**  Extern variables */
extern GUIManager st_gui;
extern pthread_cond_t cv;
extern pthread_mutex_t lock;
#endif
