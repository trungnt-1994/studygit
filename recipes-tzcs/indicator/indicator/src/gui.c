#include "gui.h"
#include <string.h>
#include <X11/extensions/Xrandr.h>
#include <stdlib.h> 

static const char *first_frame_color    = "#000000";//000000
static const char *second_frame_color   = "#444444";//444444

/**  Monitor item */
static char *monitor_icon[]        = {"/usr/share/indicator/resource/P2_Monitors/Monitor_Internal_85.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_Internal_big.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_External_85.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_External_big.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_Clone_85.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_Clone_big.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_Extend_85.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_Extend_big.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_Switch_85.png",
                                      "/usr/share/indicator/resource/P2_Monitors/Monitor_Switch_big.png"
};

/**  Network item */
static char *network_icon[]        = {"/usr/share/indicator/resource/P0_Status/Wired_small.png",
                                      "/usr/share/indicator/resource/P0_Status/Wired_big.png",
                                      "/usr/share/indicator/resource/P0_Status/Wi-Fi_small.png",
                                      "/usr/share/indicator/resource/P0_Status/Wi-Fi_big.png"
};

/**  Touchpad item */
static char *touchpad_icon[]       = {"/usr/share/indicator/resource/P5_Touchpad/Touch_ON_small.png",
                                      "/usr/share/indicator/resource/P5_Touchpad/Touch_ON_big.png",
                                      "/usr/share/indicator/resource/P5_Touchpad/Touch_OFF_small.png",
                                      "/usr/share/indicator/resource/P5_Touchpad/Touch_OFF_big.png"
};

/**  Brightness item */
static char *brightness_icon[]    = { "/usr/share/indicator/resource/P4_Brightness/B1.png",
                                      "/usr/share/indicator/resource/P4_Brightness/B2.png",
                                      "/usr/share/indicator/resource/P4_Brightness/B3.png",
                                      "/usr/share/indicator/resource/P4_Brightness/B4.png",
                                      "/usr/share/indicator/resource/P4_Brightness/B5.png",
                                      "/usr/share/indicator/resource/P4_Brightness/B6.png",
                                      "/usr/share/indicator/resource/P4_Brightness/B7.png",
                                      "/usr/share/indicator/resource/P4_Brightness/B8.png"
};

static char *brightness_mode[]     = {"/usr/share/indicator/resource/P4_Brightness/Brightness_active_unit.png",
                                      "/usr/share/indicator/resource/P4_Brightness/Brightness_inactive_unit.png"
};

/**  status */
static char *wire_stt              = "/usr/share/indicator/resource/P0_Status/Wired.png";
static char *wifi_stt[]            = {"/usr/share/indicator/resource/P0_Status/wifi_0.png",
                                      "/usr/share/indicator/resource/P0_Status/wifi_1.png",
                                      "/usr/share/indicator/resource/P0_Status/wifi_2.png",
                                      "/usr/share/indicator/resource/P0_Status/wifi_3.png",
                                      "/usr/share/indicator/resource/P0_Status/wifi_4.png"
};
static char *battery_status[]      = {"/usr/share/indicator/resource/P0_Status/Battery_0.png",
                                      "/usr/share/indicator/resource/P0_Status/Battery_25.png",
                                      "/usr/share/indicator/resource/P0_Status/Battery_50.png",
                                      "/usr/share/indicator/resource/P0_Status/Battery_75.png",
                                      "/usr/share/indicator/resource/P0_Status/Battery_100.png",
                                      "/usr/share/indicator/resource/warning/warning.png"
};

GUIManager st_gui;
pthread_cond_t cv;
pthread_mutex_t lock;

/**  Function for create GUI */
PointerHomeFrame       create_home_frame(GtkBuilder *builder);
PointerMonitorFrame    create_monitor_frame(GtkBuilder *builder, PointerMonitorInfo p_monitor_info);
PointerNetworkFrame    create_network_frame(GtkBuilder *builder, PointerNetworkInfo p_network_info);
PointerTouchPadFrame   create_touchpad_frame(GtkBuilder *builder, PointerTouchPadInfo p_touchpad_info);
PointerBrightnessFame  create_brightness_frame(GtkBuilder *builder, PointerBrightnessInfo p_brightness_info);
PointerStatusFrame     create_status_frame(GtkBuilder *builder);
#ifdef ENABLE_CTRLPANEL
PointerCtrlPanelFrame  create_ctrlpanel_frame(GtkBuilder *builder);
#endif

/**-------------------------------------------------------------------
 * <B>Function</B>:    init_frame
 * 
 * Init all frames that use on indicator application
 *
 * @return   GtkWidget *
 * @n
 *------------------------------------------------------------------*/
GtkWidget *init_frame()
//Init all frames that use on indicator application
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    GtkBuilder *builder;
    GtkWidget  *window;
    GtkWidget  *status_box;
    GtkWidget  *content_box;
    GdkColor   first_bg_color, second_bg_color;
    GdkScreen  *gds;

    gdk_color_parse(first_frame_color, &first_bg_color);
    gdk_color_parse(second_frame_color, &second_bg_color);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "/usr/share/indicator/glade/main.glade", NULL);

    window         = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    status_box     = GTK_WIDGET(gtk_builder_get_object(builder, "status_box"));
    content_box    = GTK_WIDGET(gtk_builder_get_object(builder, "content_box"));

    /* Set background color for window and 2 main boxes */
    gtk_widget_modify_bg(GTK_WIDGET(window),      GTK_STATE_NORMAL, &first_bg_color);
    gtk_widget_modify_bg(GTK_WIDGET(status_box),  GTK_STATE_NORMAL, &second_bg_color);
    gtk_widget_modify_bg(GTK_WIDGET(content_box), GTK_STATE_NORMAL, &first_bg_color);

    /* Initial status frame */
    st_gui.p_fr_status = create_status_frame(builder);
    /* Initial home frame */
    st_gui.p_fr_home = create_home_frame(builder);
    /* Initial monitor frame */
    st_gui.p_fr_monitor = create_monitor_frame(builder, &monitor_info_s);
    st_gui.p_fr_monitor->monitor_select = monitor_info_s.current_mode;
    /* Initial brightness frame */
    st_gui.p_fr_bright = create_brightness_frame(builder, &brightness_s);
    /* Initial network frame */
    st_gui.p_fr_network = create_network_frame(builder, &network_info_s);
    st_gui.p_fr_network->network_select = network_info_s.current_network_mode;
    /* Initial touch pad frame */
    st_gui.p_fr_touchpad = create_touchpad_frame(builder, &touch_pad_s);
    st_gui.p_fr_touchpad->mode_select = touch_pad_s.current_mode;
#ifdef ENABLE_CTRLPANEL
    /* Initial control panel frame */
    st_gui.p_fr_ctrlpanel = create_ctrlpanel_frame(builder);
#endif

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show(window);
    /* Init animation data */
    memset(&st_gui.st_animationdata, 0, sizeof(WindowData));
    gds = gdk_screen_get_default(); /* Get pointer to screen  */
    st_gui.st_animationdata.w = gdk_screen_get_width(gds); /* Find out screen width  */
    st_gui.st_animationdata.h = gdk_screen_get_height(gds); /* Find out screen height */
    st_gui.st_animationdata.direction = ANIMATION_DOWN;
    st_gui.st_animationdata.x = (int) st_gui.st_animationdata.w / 2 - (int) MAIN_WINDOW_WIDTH / 2;
    st_gui.st_animationdata.y = - MAIN_WINDOW_HEIGHT;
    gtk_window_move(GTK_WINDOW(window), st_gui.st_animationdata.x, st_gui.st_animationdata.y);
    /* End of init */
    st_gui.p_current_fr = st_gui.p_fr_home->frame;
    st_gui.window = window;
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return window;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    create_status_frame
 * 
 * This function is crate all components of status bar of indicator window
 *
 * @returns   PointerStatusFrame
 *
 * @param GtkBuilder * builder
 *
 *------------------------------------------------------------------*/
PointerStatusFrame create_status_frame(GtkBuilder *builder)
//This function is crate all components of status bar of indicator window
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    PointerStatusFrame p_fr_status = (PointerStatusFrame) malloc(sizeof (StatusFrame));

    p_fr_status->frame = GTK_WIDGET(gtk_builder_get_object(builder, "status_frame"));
    p_fr_status->status_img[NETWORK_STT]  = GTK_WIDGET(gtk_builder_get_object(builder, "net_index"));
    p_fr_status->status_img[BATTERY_ICON] = GTK_WIDGET(gtk_builder_get_object(builder, "bat_index"));
    p_fr_status->status_img[BATTERY_STT]  = GTK_WIDGET(gtk_builder_get_object(builder, "bat_index_percent"));
    p_fr_status->status_img[CHARGER_STT]  = GTK_WIDGET(gtk_builder_get_object(builder, "charge_index"));

    p_fr_status->p_battery_info = &battery_s;
    p_fr_status->p_network_info = &network_info_s;
    update_status_icon(p_fr_status);
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return p_fr_status;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    create_home_frame
 * 
 * Create all components of monitor frame
 *
 * @returns   PointerHomeFrame
 *
 * @param GtkBuilder * builder
 * @n
 *------------------------------------------------------------------*/
PointerHomeFrame create_home_frame(GtkBuilder *builder)
//Create all components of monitor frame
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    PointerHomeFrame p_home_frame = (PointerHomeFrame) malloc (sizeof(HomeFrame));
    /* Init data */
    p_home_frame->home_status[MONITOR_ITEM]    = (int *) &monitor_info_s.current_mode;
    p_home_frame->home_status[BRIGHTNESS_ITEM] = (int *) &brightness_s.current_value;
    p_home_frame->home_status[NETWORK_ITEM]    = (int *) &network_info_s.current_network_mode;
    p_home_frame->home_status[TOUCHPAD_ITEM]   = (int *) &touch_pad_s.current_mode;
    /* Gets frame components */
    p_home_frame->frame = GTK_WIDGET(gtk_builder_get_object(builder, "home_frame"));
    p_home_frame->home_img[MONITOR_ITEM]     = GTK_WIDGET(gtk_builder_get_object(builder, "monitor_img_home"));
    p_home_frame->home_img[BRIGHTNESS_ITEM]  = GTK_WIDGET(gtk_builder_get_object(builder, "brightness_img_home"));
    p_home_frame->home_img[NETWORK_ITEM]     = GTK_WIDGET(gtk_builder_get_object(builder, "network_img_home"));
    p_home_frame->home_img[TOUCHPAD_ITEM]    = GTK_WIDGET(gtk_builder_get_object(builder, "touchpad_img_home"));
    p_home_frame->ctrlpanel_box              = GTK_WIDGET(gtk_builder_get_object(builder, "ctrlpanel_home"));

#ifdef ENABLE_CTRLPANEL
    gtk_widget_show_all(GTK_WIDGET(p_home_frame->ctrlpanel_box));
#endif

    gtk_image_set_from_file(GTK_IMAGE(p_home_frame->home_img[MONITOR_ITEM]),
            monitor_icon[*(p_home_frame->home_status[MONITOR_ITEM])]);
    gtk_image_set_from_file(GTK_IMAGE(p_home_frame->home_img[BRIGHTNESS_ITEM]),
            brightness_icon[*(p_home_frame->home_status[BRIGHTNESS_ITEM]) - 1]);
    gtk_image_set_from_file(GTK_IMAGE(p_home_frame->home_img[NETWORK_ITEM]),
            network_icon[2*(*(p_home_frame->home_status[NETWORK_ITEM]))]);
    gtk_image_set_from_file(GTK_IMAGE(p_home_frame->home_img[TOUCHPAD_ITEM]),
            touchpad_icon[2*(*(p_home_frame->home_status[TOUCHPAD_ITEM]))]);

#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return p_home_frame;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    create_monitor_frame
 * 
 * Create all components of monitor frame
 *
 * @returns   PointerMonitorFrame
 *
 * @param GtkBuilder * builder
 * @param PointerMonitorInfo p_monitor_info
 * @n
 *------------------------------------------------------------------*/
PointerMonitorFrame create_monitor_frame(GtkBuilder *builder, PointerMonitorInfo p_monitor_info)
//Create all components of monitor frame
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    PointerMonitorFrame p_monitor_frame = (PointerMonitorFrame) malloc (sizeof(MonitorFrame));
    /* Init data*/
    p_monitor_frame->p_monitor_info = p_monitor_info;
    /* Gets frame components*/
    p_monitor_frame->frame = GTK_WIDGET(gtk_builder_get_object(builder, "monitor_frame"));

    p_monitor_frame->monitor_frame[0] = GTK_WIDGET(gtk_builder_get_object(builder, "internal_frame"));
    p_monitor_frame->monitor_frame[1] = GTK_WIDGET(gtk_builder_get_object(builder, "external_frame"));
    p_monitor_frame->monitor_frame[2] = GTK_WIDGET(gtk_builder_get_object(builder, "clone_frame"));
    p_monitor_frame->monitor_frame[3] = GTK_WIDGET(gtk_builder_get_object(builder, "extend_frame"));
    p_monitor_frame->monitor_frame[4] = GTK_WIDGET(gtk_builder_get_object(builder, "switch_frame"));

    p_monitor_frame->monitor_img[0] = GTK_WIDGET(gtk_builder_get_object(builder, "internal_icon"));
    p_monitor_frame->monitor_img[1] = GTK_WIDGET(gtk_builder_get_object(builder, "external_icon"));
    p_monitor_frame->monitor_img[2] = GTK_WIDGET(gtk_builder_get_object(builder, "clone_icon"));
    p_monitor_frame->monitor_img[3] = GTK_WIDGET(gtk_builder_get_object(builder, "extend_icon"));
    p_monitor_frame->monitor_img[4] = GTK_WIDGET(gtk_builder_get_object(builder, "switch_icon"));

    /* Update frame components following monitor data */
    update_extend_monitor(p_monitor_frame);

    /* Update frame components following monitor data */
    int index;
    index = (int) p_monitor_frame->p_monitor_info->current_mode;
    gtk_image_set_from_file(GTK_IMAGE(p_monitor_frame->monitor_img[index]), monitor_icon[2*index+1]);
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__, __FILE__);
#endif
    return p_monitor_frame;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    create_brightness_frame
 * 
 * Create all components of brightness frame
 *
 * @returns   PointerBrightnessFame
 *
 * @param GtkBuilder * builder
 * @param PointerBrightnessInfo p_brightness_info
 * @n
 *------------------------------------------------------------------*/
PointerBrightnessFame create_brightness_frame(GtkBuilder *builder, PointerBrightnessInfo p_brightness_info )
//Create all components of brightness frame
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    PointerBrightnessFame p_brightness_frame = (PointerBrightnessFame) malloc (sizeof(BrightnessFame));

    /* Init data */
    p_brightness_frame->p_brightness_info = p_brightness_info;
    /* Frame */
    p_brightness_frame->frame = GTK_WIDGET(gtk_builder_get_object(builder, "brightness_frame"));
    int idex;
    for (idex = 0; idex < BRIGHTNESS_ITEM_NUMBER; idex++) {
        char brg_scale_id[20];
        memset(brg_scale_id, '\0', sizeof(brg_scale_id));
        sprintf(brg_scale_id, "brg_scale_%d", idex);
        p_brightness_frame->brightness_img[idex] = GTK_WIDGET(gtk_builder_get_object(builder, brg_scale_id));
        gtk_image_set_from_file(GTK_IMAGE(p_brightness_frame->brightness_img[idex]), brightness_mode[ACTIVE]);
    }
    /* Update current brightness */
    change_frame_brightness_icon(p_brightness_frame);
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return p_brightness_frame;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    create_network_frame
 * 
 * Create all components of network frame
 *
 * @returns   PointerNetworkFrame
 *
 * @param GtkBuilder * builder
 * @param PointerNetworkInfo p_network_info
 * @n
 *------------------------------------------------------------------*/
PointerNetworkFrame create_network_frame(GtkBuilder *builder, PointerNetworkInfo p_network_info)
//Create all components of network frame
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    PointerNetworkFrame p_network_frame = (PointerNetworkFrame) malloc (sizeof(NetworkFrame));
    /* Init data */
    p_network_frame->p_network_info = p_network_info;
    /* Init frame components */
    p_network_frame->frame = GTK_WIDGET(gtk_builder_get_object(builder, "network_frame"));
    p_network_frame->network_img[0] = GTK_WIDGET(gtk_builder_get_object(builder, "wired_icon"));
    p_network_frame->network_img[1] = GTK_WIDGET(gtk_builder_get_object(builder, "wifi_icon"));
    int idex;
    /* Update frame components following network data */
    idex = (int) p_network_frame->p_network_info->current_network_mode;
    gtk_image_set_from_file(GTK_IMAGE(p_network_frame->network_img[idex]), network_icon[2*idex+1]);
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return p_network_frame;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    create_touchpad_frame
 * 
 * Create all components of touch pad frame
 *
 * @returns   PointerTouchPadFrame
 *
 * @param GtkBuilder * builder
 * @param PointerTouchPadInfo p_touchpad_info
 * @n
 *------------------------------------------------------------------*/
PointerTouchPadFrame create_touchpad_frame(GtkBuilder *builder, PointerTouchPadInfo p_touchpad_info)
//Create all components of touch pad frame
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    PointerTouchPadFrame p_touchpad_frame = (PointerTouchPadFrame) malloc(sizeof(TouchPadFrame));
    /* Init data */
    p_touchpad_frame->p_touchpad_info = p_touchpad_info;
    /* Create frame components */
    p_touchpad_frame->frame = GTK_WIDGET(gtk_builder_get_object(builder, "touchpad_frame"));
    p_touchpad_frame->touchpad_img[0] = GTK_WIDGET(gtk_builder_get_object(builder, "touchpad_on_icon"));
    p_touchpad_frame->touchpad_img[1] = GTK_WIDGET(gtk_builder_get_object(builder, "touchpad_off_icon"));
    int idex;
    /* Update frame components following touch pad data */
    idex = (int) p_touchpad_frame->p_touchpad_info->current_mode;
    gtk_image_set_from_file(GTK_IMAGE(p_touchpad_frame->touchpad_img[idex]), touchpad_icon[2*idex+1]);
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return p_touchpad_frame;
}

#ifdef ENABLE_CTRLPANEL
/**-------------------------------------------------------------------
 * <B>Function</B>:    create_ctrlpanel_frame
 *
 * Create all components of controlpanel frame
 *
 * @returns   PointerCtrlPanelFrame
 *
 * @param GtkBuilder * builder
 * @n
 *------------------------------------------------------------------*/
PointerCtrlPanelFrame create_ctrlpanel_frame(GtkBuilder *builder)
//Create all components of controlpanel frame
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    PointerCtrlPanelFrame p_ctrlpanel_frame = (PointerCtrlPanelFrame) malloc(sizeof(CtrlPanelFrame));
    /* Create frame components */
    p_ctrlpanel_frame->frame = GTK_WIDGET(gtk_builder_get_object(builder, "ctrlpanel_frame"));
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return p_ctrlpanel_frame;
}
#endif

/**-------------------------------------------------------------------
 * <B>Function</B>:    update_window_data
 * 
 * Update window data when user plug/unplug VGA/HDMI
 *
 * @n
 *------------------------------------------------------------------*/
void update_window_data()
//Update window data when user plug/unplug VGA/HDMI
{
#ifdef DEBUG_GUI

    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
    printf("Window position before update x = %d: y=%d \n", st_gui.st_animationdata.x,st_gui.st_animationdata.y);
#endif
    /** Update Indicator position*/
    int new_x_position = 0;
    int new_y_position = 0;
    pOutputs p_outputs;
    int i;
    p_outputs = get_screen_info();
    /** Get windows size of Indicator */
    int width, height;
    gtk_window_get_size (st_gui.window, &width, &height);
    printf("Size of window : width = %d, heigh = %d\n", width, height);
    if (p_outputs == NULL){
        printf ("Out put is null \n");
    }
    else{
        st_gui.screen1_width = p_outputs->out_screens[p_outputs->ninternal].width;
        st_gui.screen2_width = p_outputs->out_screens[p_outputs->nsecond].width;

        switch (st_gui.p_fr_monitor->p_monitor_info->extend_type){
        case profile_single:
            new_x_position = st_gui.screen1_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_external:
            new_x_position = st_gui.screen2_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_clone:
            new_x_position = st_gui.screen2_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_extend_below:
            new_x_position = st_gui.screen1_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_extend_above:
            new_x_position = st_gui.screen2_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_extend_right:
        case profile_switch_left:
            new_x_position = st_gui.screen1_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_switch_below:
            new_x_position = st_gui.screen2_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_switch_above:
            new_x_position = st_gui.screen1_width/2 - width/2;
            new_y_position = 0;
            break;

        case profile_extend_left:
        case profile_switch_right:
            new_x_position = st_gui.screen2_width + st_gui.screen1_width/2 - width/2;
            new_y_position = 0;
            break;
        default:
            break;
        }
        /* Update animation data */
        st_gui.st_animationdata.x = new_x_position;
        st_gui.st_animationdata.y = new_y_position - height;

        /* Release output resource */
        for (i = 0; i < p_outputs->nscreen; i++) {
            free(p_outputs->out_screens->name);
        }
        free (p_outputs->out_screens);
    }
#ifdef DEBUG_GUI
    printf("Window showing (1:TRUE|0:FALSE): %d\n", st_gui.st_animationdata.is_showing);
    printf("Window switching (1:TRUE|0:FALSE): %d\n", st_gui.p_fr_monitor->p_monitor_info->is_switching);
    printf("Window position after update_screen_size: x = %d: y=%d \n", st_gui.st_animationdata.x,st_gui.st_animationdata.y);
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    show_frame
 * 
 * Determine the frame will be show base on item type
 *
 * @param int frame_item
 *  @n
 *------------------------------------------------------------------*/
void show_frame(int frame_item)
//Determine the frame will be show base on item type
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    gdk_threads_enter();
    /* Show frame */
    switch (frame_item) {
    case MONITOR_FRAME_TYPE:
        if (st_gui.current_frame != MONITOR_FRAME_TYPE) {
            gtk_widget_hide(st_gui.p_current_fr);
            gtk_widget_show(st_gui.p_fr_monitor->frame);
            st_gui.p_current_fr = st_gui.p_fr_monitor->frame;
        } else {
            change_frame_monitor_icon(st_gui.p_fr_monitor);
        }
        break;

    case BRIGHTNESS_FRAME_TYPE:
        change_frame_brightness_icon(st_gui.p_fr_bright);
        if (st_gui.current_frame!= BRIGHTNESS_FRAME_TYPE) {
            gtk_widget_hide(st_gui.p_current_fr);
            gtk_widget_show(st_gui.p_fr_bright->frame);
            st_gui.p_current_fr = st_gui.p_fr_bright->frame;
        }
        break;

    case NETWORK_FRAME_TYPE:
        if (st_gui.current_frame!= NETWORK_FRAME_TYPE) {
            gtk_widget_hide(st_gui.p_current_fr);
            gtk_widget_show(st_gui.p_fr_network->frame);
            st_gui.p_current_fr = st_gui.p_fr_network->frame;
        } else {
            change_frame_network_icon(st_gui.p_fr_network);
        }
        break;

    case TOUCHPAD_FRAME_TYPE:
        if (st_gui.current_frame!= TOUCHPAD_FRAME_TYPE) {
            gtk_widget_hide(st_gui.p_current_fr);
            gtk_widget_show(st_gui.p_fr_touchpad->frame);
            st_gui.p_current_fr = st_gui.p_fr_touchpad->frame;
        } else {
            change_frame_touchpad_icon(st_gui.p_fr_touchpad);
        }
        break;

    case HOME_FRAME_TYPE:
        if (st_gui.current_frame != HOME_FRAME_TYPE) {
            gtk_widget_hide(st_gui.p_current_fr);
            gtk_widget_show(st_gui.p_fr_home->frame);
            st_gui.p_current_fr = st_gui.p_fr_home->frame;
        }
        break;

#ifdef ENABLE_CTRLPANEL
    case CTRLPANEL_FRAME_TYPE:
        if (st_gui.current_frame != CTRLPANEL_FRAME_TYPE) {
            gtk_widget_hide(st_gui.p_current_fr);
            gtk_widget_show(st_gui.p_fr_ctrlpanel->frame);
            st_gui.p_current_fr = st_gui.p_fr_ctrlpanel->frame;
        }
        break;
#endif

    default:
        break;
    }
    /* Update current item */
    st_gui.current_frame = frame_item;
    gdk_threads_leave();
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    show_animation
 * 
 * Show/Hide animation window
 *
 * @returns   gboolean
 *
 * @param void * window_data
 * @n
 *------------------------------------------------------------------*/
gboolean show_animation(void *window_data)
//Show/Hide animation window
{
    #ifdef DEBUG_GUI
    //printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
    #endif
    int return_status = TRUE;
    gdk_threads_enter();
    WindowData *data = (WindowData *)window_data;
    /* Set property for show window */
    gtk_window_present(GTK_WINDOW(st_gui.window));
    gtk_window_set_opacity(GTK_WINDOW(st_gui.window),1);
    data->is_animation = TRUE;
    if (data->direction == ANIMATION_DOWN) {
        if (data->y < 0) {
            data->y = data->y + ANIMATION_STEP;
            gtk_window_move(GTK_WINDOW(st_gui.window),data->x, data->y);
            data->is_showing = TRUE;
        } else {
            data->direction = ANIMATION_HIDE;
            data->timer_show_id = -1;
            data->is_animation = FALSE;
            return_status = FALSE;
        }
    } else if (data->direction == ANIMATION_UP) {
        if (data->y + MAIN_WINDOW_HEIGHT > 0) {
                data->y = data->y - ANIMATION_STEP;
                data->is_showing = TRUE;
                gtk_window_move(GTK_WINDOW(st_gui.window),data->x, data->y);
        } else {
            data->timer_show_id = -1;
            data->direction= ANIMATION_HIDE;
            data->is_animation = FALSE; 
            data->is_showing = FALSE;
            gtk_window_set_opacity(GTK_WINDOW(st_gui.window),0);
            return_status = FALSE;
            pthread_cond_signal(&cv);
        }
    }
    gdk_threads_leave();
    return return_status;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    update_extend_monitor
 * 
 * This function do update monitor frame for suitable with current monitor mode
 *
 * @param PointerMonitorFrame p_monitor
 * @n
 *------------------------------------------------------------------*/
void update_extend_monitor(PointerMonitorFrame p_monitor)
//This function do update monitor frame for suitable with current monitor mode
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    int index;
    if ((p_monitor->p_monitor_info->un_plugVGA)&&(p_monitor->p_monitor_info->un_plugHDMI)) {
        /* Only show Internal item */
        gtk_widget_set_size_request(GTK_WIDGET(p_monitor->monitor_img[0]), MAIN_WINDOW_WIDTH, CONTENT_FRAME_HEIGHT);
        for (index = 1; index < MONITOR_ITEM_NUMBER; index++) {
            gtk_widget_hide(GTK_WIDGET(p_monitor->monitor_frame[index]));
        }
    } else {
#ifndef DISABLE_SWITCHMODE
        if (p_monitor->p_monitor_info->current_mode > CLONE) {
            for (index = 0; index < MONITOR_ITEM_NUMBER; index++) {
                gtk_widget_show_all(GTK_WIDGET(p_monitor->monitor_frame[index]));
                gtk_widget_set_size_request(GTK_WIDGET(p_monitor->monitor_img[index]), ITEM_SWITCH_WIDTH, CONTENT_FRAME_HEIGHT);
                /* Reset ico/n */
                gtk_image_set_from_file(GTK_IMAGE(p_monitor->monitor_img[index]), monitor_icon[2*index]);
            }
        } else {
#endif  // Define DISABLE_SWITCHMODE
            gtk_widget_hide(GTK_WIDGET(p_monitor->monitor_frame[MONITOR_ITEM_NUMBER - 1]));
            for (index = 0; index < MONITOR_ITEM_NUMBER - 1; index++) {
                gtk_widget_show_all(GTK_WIDGET(p_monitor->monitor_frame[index]));
                gtk_widget_set_size_request(GTK_WIDGET(p_monitor->monitor_img[index]), ITEM_EXTEND_WIDTH, CONTENT_FRAME_HEIGHT);
                /* Reset icon */
                gtk_image_set_from_file(GTK_IMAGE(p_monitor->monitor_img[index]), monitor_icon[2*index]);
            }
#ifndef DISABLE_SWITCHMODE
        }
#endif
    }
    /* Update icon */
    p_monitor->monitor_select = p_monitor->p_monitor_info->current_mode;
    index = p_monitor->monitor_select;
    gtk_image_set_from_file(GTK_IMAGE(p_monitor->monitor_img[index]), monitor_icon[2*index+1]);
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    change_frame_network_icon
 * 
 * This function do update network frame when user change network mode
 *
 * @param PointerNetworkFrame p_network
 * @n
 *------------------------------------------------------------------*/
void change_frame_network_icon(PointerNetworkFrame p_network) 
//This function do update network frame when user change network mode
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    if (!p_network->p_network_info->is_switching) {
        /* Reset mode */
        int idex = (int) p_network->network_select;
        gtk_image_set_from_file(GTK_IMAGE(p_network->network_img[idex]), network_icon[2*idex]);
        /* Update current mode */
        if (p_network->network_select == WIRE_MODE) {
            p_network->network_select = WIFI_MODE;
        } else if (p_network->network_select == WIFI_MODE) {
            p_network->network_select = WIRE_MODE;
        }
        /* Update current mode */
        idex = (int) p_network->network_select;
        gtk_image_set_from_file(GTK_IMAGE(p_network->network_img[idex]), network_icon[2*idex+1]);
    }
}


/**-------------------------------------------------------------------
 * <B>Function</B>:    change_frame_brightness_icon
 * 
 * This function do update brightness frame when user change brightness level
 *
 * @param PointerBrightnessFame p_brightness
 * @n
 *------------------------------------------------------------------*/
void change_frame_brightness_icon(PointerBrightnessFame p_brightness) 
//This function do update brightness frame when user change brightness level
{
    int idex;
    int level = p_brightness->p_brightness_info->current_value;
    for (idex = 0; idex < BRIGHTNESS_ITEM_NUMBER; idex++) {
        if (idex < level) {
            gtk_image_set_from_file(GTK_IMAGE(p_brightness->brightness_img[idex]), brightness_mode[ACTIVE]);
        } else {
            gtk_image_set_from_file(GTK_IMAGE(p_brightness->brightness_img[idex]), brightness_mode[INACTIVE]);
        }
    }
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    change_frame_monitor_icon
 * 
 * This function do update monitor frame when user change monitor mode
 *
 * @param PointerMonitorFrame p_monitor
 * @n
 *------------------------------------------------------------------*/
void change_frame_monitor_icon(PointerMonitorFrame p_monitor)
//This function do update monitor frame when user change monitor mode
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    int i_position;
    if ((!p_monitor->p_monitor_info->un_plugVGA)||(!p_monitor->p_monitor_info->un_plugHDMI)) {
        /* Reset current mode */
        i_position = p_monitor->monitor_select;
        gtk_image_set_from_file(GTK_IMAGE(p_monitor->monitor_img[i_position]), monitor_icon[2*i_position]);
        /* Switch to new mode */
        switch (p_monitor->monitor_select) {
        case SWITCH:
            p_monitor->monitor_select = INTERNAL;
            break;

        case INTERNAL:
            p_monitor->monitor_select = EXTERNAL;
            break;
        
        case EXTERNAL:
            p_monitor->monitor_select = CLONE;
            break;

        case CLONE:
            p_monitor->monitor_select = EXTEND;
            break;
        
        case EXTEND:
#ifndef DISABLE_SWITCHMODE
            if (p_monitor->p_monitor_info->current_mode > CLONE) {
                p_monitor->monitor_select = SWITCH;
            } else {
#endif
                p_monitor->monitor_select = INTERNAL;
#ifndef DISABLE_SWITCHMODE
            }
#endif
            break;

        default:
            break;
        }
        i_position = p_monitor->monitor_select;
        gtk_image_set_from_file(GTK_IMAGE(p_monitor->monitor_img[i_position]), monitor_icon[2*i_position+1]);
    }
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    change_frame_touchpad_icon
 * 
 * This function do update touch pad frame when user change touch pad mode
 *
 * @param PointerTouchPadFrame p_touchpad
 * @n
 *------------------------------------------------------------------*/
void change_frame_touchpad_icon(PointerTouchPadFrame p_touchpad)
//This function do update touch pad frame when user change touch pad mode
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    /* Reset mode */
    int idex = (int) p_touchpad->mode_select;
    gtk_image_set_from_file(GTK_IMAGE(p_touchpad->touchpad_img[idex]), touchpad_icon[2*idex]);
    /* Update current mode */
    if (p_touchpad->mode_select == TOUCH_OFF) {
        p_touchpad->mode_select = TOUCH_ON;
    } else if (p_touchpad->mode_select == TOUCH_ON) {
        p_touchpad->mode_select = TOUCH_OFF;
    }
    /* Update current mode */
    idex = (int) p_touchpad->mode_select;
    gtk_image_set_from_file(GTK_IMAGE(p_touchpad->touchpad_img[idex]),touchpad_icon[2*idex+1]);
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    update_status_icon
 * 
 * Update icon on frame status
 *
 * @param PointerStatusFrame p_status
 * @n
 *------------------------------------------------------------------*/
void update_status_icon(PointerStatusFrame p_status)
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    int level;
    /* Get network icon */
    if (p_status->p_network_info->current_network_mode == WIRE_MODE) {
        gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[NETWORK_STT]), wire_stt);
    } else {
        level = p_status->p_network_info->wifi_setting.wifi_index / 25;
        if (level > 4) {
            gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[NETWORK_STT]), wifi_stt[4]);
        } else {
            gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[NETWORK_STT]), wifi_stt[level]);
        }
    }
    /* Get battery icon */
    char str_battery_capacity[10];
    level = p_status->p_battery_info->battery_index;
    snprintf(str_battery_capacity,10,"%d",level);
    strcat(str_battery_capacity,"%");
    if(level <= 10 ) {
        gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[BATTERY_ICON]), battery_status[0]);
    } else if(level > 10 && level <= 35 ) {
        gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[BATTERY_ICON]), battery_status[1]);
    } else if(level > 35 && level <= 65 ) {
        gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[BATTERY_ICON]), battery_status[2]);
    } else if(level > 65 && level <= 85 ) {
        gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[BATTERY_ICON]), battery_status[3]);
    }  else if(level > 85 && level <= 100 ) {
        gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[BATTERY_ICON]), battery_status[4]);
    } else {
        gtk_image_set_from_file(GTK_IMAGE(p_status->status_img[BATTERY_ICON]), battery_status[5]);
        strcpy(str_battery_capacity,"Error");
    }
    gtk_label_set_markup(GTK_LABEL(p_status->status_img[BATTERY_STT]), str_battery_capacity);
    /* Get charging icon */
    if (p_status->p_battery_info->battery_charge != TRUE ) {
        gtk_widget_hide_all(GTK_WIDGET(p_status->status_img[CHARGER_STT]));
    } else {
        gtk_widget_show_all(GTK_WIDGET(p_status->status_img[CHARGER_STT]));
    }
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif // DEBUG_GUI
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    update_home_frame
 * 
 * This function update icon path of home frame after each user change
 *@n
 *------------------------------------------------------------------*/
void update_home_frame()
//This function update icon path of home frame after each user change
{
#ifdef DEBUG_GUI
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    /* Init data */
    GtkWidget *image;
    char *new_image_path;
    int index;
    int position;
    for (index = 0; index < HOME_ITEM_NUMBER; index++) {
        image = st_gui.p_fr_home->home_img[index];
        position = *st_gui.p_fr_home->home_status[index];
        switch (index) {
        case MONITOR_ITEM:
            if (position >= EXTEND){
                position = EXTEND;
            }
            new_image_path = monitor_icon[2*position];
            break;

        case BRIGHTNESS_ITEM:
            new_image_path = brightness_icon[position-1];
            break;

        case NETWORK_ITEM:
            new_image_path = network_icon[2*position];
            break;

        case TOUCHPAD_ITEM:
            new_image_path = touchpad_icon[2*position];
            break;

        default:
            break;
        }
        gtk_image_set_from_file(GTK_IMAGE(image), new_image_path);
    }
#ifdef DEBUG_GUI
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    update_network_icon
 * 
 * Update network icon for suitable with current network mode
 *@n
 *------------------------------------------------------------------*/
void update_network_icon()
//Update network icon for suitable with current network mode
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    int idex;
    if (st_gui.p_fr_network->network_select != network_info_s.current_network_mode) {
        /* Reset network icon */
        for (idex = 0; idex < NETWORK_ITEM_NUMBER; idex++) {
            gtk_image_set_from_file(GTK_IMAGE(st_gui.p_fr_network->network_img[idex]), network_icon[2*idex]);
        }
        /* Update frame components following network data */
        idex = (int) st_gui.p_fr_network->p_network_info->current_network_mode;
        gtk_image_set_from_file(GTK_IMAGE(st_gui.p_fr_network->network_img[idex]), network_icon[2*idex+1]);
        /* Update network selection mode */
        st_gui.p_fr_network->network_select = network_info_s.current_network_mode;
    }
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
}

/**-------------------------------------------------------------------
 * <B>Function</B>:update_touchpad_icon
 * 
 * This function do reset touch pad frame when user cancel the change
 *
 * @returns   void
 *
 * @n
 *------------------------------------------------------------------*/
void update_touchpad_icon()
    //This function do reset touch pad frame when user cancel the change
{
#ifdef TOUCHPAD_DEBUG
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    int idex;
    if (st_gui.p_fr_touchpad->mode_select != touch_pad_s.current_mode) {
        /* Reset touch pad icon */
        for (idex = 0; idex < TOUCHPAD_ITEM_NUMBER; idex++) {
            gtk_image_set_from_file(GTK_IMAGE(st_gui.p_fr_touchpad->touchpad_img[idex]), touchpad_icon[2*idex]);
        }
        /* Update frame components following network data */
        idex = (int) touch_pad_s.current_mode;
        gtk_image_set_from_file(GTK_IMAGE(st_gui.p_fr_touchpad->touchpad_img[idex]), touchpad_icon[2*idex+1]);
        /* Update network selection mode */
        st_gui.p_fr_touchpad->mode_select = touch_pad_s.current_mode;
    }
#ifdef TOUCHPAD_DEBUG
    printf("Leave to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
}
