/** **************************************************
* File:  indicator.c
* Author: TSDV
* Created on November 19, 2014, 4:39 PM
****************************************************/
#include <gdk/gdkkeysyms.h> /* for independently test GUI*/
#include <gtk/gtk.h>
#include <glib.h>
#include <X11/extensions/Xrandr.h> /*for Xthread Init */
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <locale.h> /* for setlocale */
#include <linux/unistd.h>
#include <pthread.h>
#include <libudev.h> /* for detect VGA */
#include <libintl.h> /* for bindtextdomain and textdomain */
/**  TZCS environment */
#include <tzcslib/TzcsUtil.h>
#include <libtzcs-security/libtzcs-security.h>
#include "monitor.h"
#include "network.h"
#include "brightness.h"
#include "battery.h"
#include "helper.h"
#include "touchpad.h"
#include "ctrlpanel.h"
#include "gui.h"
#include "hdmi_certificate.h"

#if HAVE_GETTEXT
#define _(String) gettext(String)
#else
#define _(String) String
#endif

#define SET_SELECTDEV _IOW('q', 1, int)
#define TP_ON 0x1
#define SP_ON 0x2
#define PROCFS_FILE_NAME_PATH "/proc/indicatorproc"
#define SIG_INDICATOR_SIZE (44)

const char *send_logout_event = "xdotool windowactivate `xdotool search --onlyvisible --name \"Wfica\" | tail -1`;xdotool key super+l\n\
                                 xdotool windowactivate `xdotool search --onlyvisible --name \"Wfica\" | head -1`;xdotool key super+l";
// const char *setlayer_indicator = "xdotool windowactivate `xdotool search --name \"indicator\" | tail -1` &";
const char *getinput = "grep  -E 'Handlers|EV=' /proc/bus/input/devices | grep -B1 'EV=120013' | grep -Eo 'event[0-9]+' | tail -n 1";
/**  Signal structure */
typedef struct _EventStructure
{
    int                config_fd;
    struct sigaction   sigPlugIn;
    pthread_t          p_thread_detect_VGA;
} EvenStructure;

/** --------------------------------------------------------------------
 * Global variable
 *------------------------------------------------------------------*/
EvenStructure st_event ={0};
pthread_t  thread_system_handle, thread_signal_event, thread_update_network;

void monitor_system_status();
void send_notification_event(int n, siginfo_t *info, void *unused);
void handle_sig_event(int signal_event);
void signal_event_handler(void *thread_arg);
void handle_system_display(void);
static void *update_network_thread(void *arg);
/**-------------------------------------------------------------------
 * <B>Function</B>:    main
 *
 * Main function initialize and control application
 *
 * @return        OK : successful
 * @return        NG : Some error happen
 *
 * @param int argc
 * @param char * argv[]
 * @n
 *------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    /*
     * Init component
     */
    /* Init touchpad status */
    get_touchpad_status();
    /* Init battery status */
    get_battery_info();
    /* Init brightness info */
    get_brightness();
    /* Init network info */
    init_network();
    /* Monitor states initial*/
    init_monitor_status();
    /* Init X thread*/
    XInitThreads();

    /* Set */
    get_key_pressed_BIOS(H_CODE);
    setlocale(LC_ALL,"");
    bindtextdomain("indicator","/usr/share/locale");
    textdomain("indicator");
    int ret = init_hardware_driver();
    if(EXIT_SUCCESS != ret) {
        printf("Not found /dev/tvalz0\n");
    }
    /* Init all thread */
    pthread_create(&thread_signal_event,NULL,(void *)&signal_event_handler,NULL);
    handle_system_display();
    pthread_join(thread_signal_event, NULL);
    return 0;
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    monitor_system_status
 *
 * Update VGA/HDMI status and battery, charge, network status
 * @n
 *------------------------------------------------------------------*/
void monitor_system_status() // Update VGA/HDMI status and battery, charge, network status
{
    struct timeval tv;
    struct udev *udev;
    struct udev_device *dev;
    udev = udev_new();
    if (!udev) {
        printf("Can't create udev\n");
        exit(1);
    }
    /* Register the VGA monitor */
    struct udev_monitor *monitor_VGA = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(monitor_VGA, "drm", NULL);
    udev_monitor_enable_receiving(monitor_VGA);
    int fd = udev_monitor_get_fd(monitor_VGA);
    while (1) {
        fd_set fds;
        int ret;
        /* Set timer to update status */
        tv.tv_sec = UPDATE_STATUS_INTERVAL;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        ret = select(fd+1, &fds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            dev = udev_monitor_receive_device(monitor_VGA);
            if (dev) {
#ifdef DEBUG_UDEV
                printf("Detect VGA event\n");
#endif // DEBUG_UDEV
                udev_device_unref(dev);
                /*Avoid issue that kernel can't detect HDMI or external monitor immediately*/
                sleep(1);
                handle_sig_event(VGA_NOTIFY);
            }
        }
#ifdef DEBUG_UDEV
        else {
            printf("Not detect VGA event\n");
        }
#endif // DEBUG_UDEV
        handle_sig_event(UPDATE_STT_NOTIFY);
    }
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    send_notification_event
 *
 *Action when event thread receive a notification from kernel
 *
 *
 * @param int n
 * @param siginfo_t * info
 * @param void * unused
 * @n
 *------------------------------------------------------------------*/
void send_notification_event(int n, siginfo_t *info, void *unused)
//Action when event thread receive a notification from kernel
{
    /* Send notifier event */
    handle_sig_event(info->si_int);
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    handle_sig_event
 *
 * Handle for received event
 *
 *
 * @param int signal_event
 * @n
 *------------------------------------------------------------------*/
void handle_sig_event(int signal_event)
// Handle for received event
{
#ifdef ENABLE_CTRLPANEL
    int ctrlpanel_status = get_ctrlpanel_status();
#endif
    switch (signal_event) { /* handle for press keyboard event*/
#ifdef ENABLE_CTRLPANEL
    case FN_F2:
        show_frame(CTRLPANEL_FRAME_TYPE);
        break;
#endif

    case FN_F3:
        system("amixer sset Master 1\%- unmute\n\
            amixer sset Speaker unmute\n\
            amixer sset Headphone unmute");
        break;

    case FN_F4:
        system("amixer sset Master 1\%+ unmute\n\
            amixer sset Speaker unmute\n\
            amixer sset Headphone unmute");
        break;
    case FN_F5:
#ifdef ENABLE_CTRLPANEL
        if(ctrlpanel_status == 1){
#endif
            show_frame(MONITOR_FRAME_TYPE);
#ifdef ENABLE_CTRLPANEL
        }
#endif
        break;

    case FN_F6:
#ifdef ENABLE_CTRLPANEL
        if(ctrlpanel_status == 1){
#endif
            descrease_brightness_BIOS();
            show_frame(BRIGHTNESS_FRAME_TYPE);
#ifdef ENABLE_CTRLPANEL
        }
#endif
        break;

    case FN_F7:
#ifdef ENABLE_CTRLPANEL
        if(ctrlpanel_status == 1){
#endif
            increase_brightness_BIOS();
            show_frame(BRIGHTNESS_FRAME_TYPE);
#ifdef ENABLE_CTRLPANEL
        }
#endif
        break;

    case FN_F8:
#ifdef ENABLE_CTRLPANEL
        if(ctrlpanel_status == 1){
#endif
            show_frame(NETWORK_FRAME_TYPE);
#ifdef ENABLE_CTRLPANEL
        }
#endif
        break;

    case FN_F9:
#ifdef ENABLE_CTRLPANEL
        if(ctrlpanel_status == 1){
#endif
            show_frame(TOUCHPAD_FRAME_TYPE);
#ifdef ENABLE_CTRLPANEL
        }
#endif
        break;

    case FN_PRESS:
#ifdef DEBUG
        printf("FN_PRESS %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
        printf("Window position : x = %d: y=%d \n", st_gui.st_animationdata.x,st_gui.st_animationdata.y);
        printf("current output = %d: \n", st_gui.p_fr_monitor->p_monitor_info->current_output);
        printf("current mode =%d \n", st_gui.p_fr_monitor->p_monitor_info->current_mode);
        printf("VGA/HDMI status = %s\n", st_gui.p_fr_monitor->p_monitor_info->un_plugVGA == FALSE?"In":"Out");
#endif
        /** Start synchronize monitor mode */
        synchronize_monitor_status();
        gdk_threads_enter();
        /* Update monitor frame */
        update_extend_monitor(st_gui.p_fr_monitor);
        /* Update window position */
        update_window_data();
        /* Update home frame */
        update_home_frame();
        gdk_threads_leave();
        /** End synchronize monitor mode */
#ifdef ENABLE_CTRLPANEL
        if(ctrlpanel_status == 0){
            show_frame(CTRLPANEL_FRAME_TYPE);
        }else{
#endif
            show_frame(HOME_FRAME_TYPE);
#ifdef ENABLE_CTRLPANEL
        }
#endif
            st_gui.st_animationdata.direction = ANIMATION_DOWN;
            if( st_gui.st_animationdata.is_animation == FALSE ) {
                st_gui.st_animationdata.timer_show_id = g_timeout_add(ANIMATION_INTERVAL, show_animation,
                        &st_gui.st_animationdata);
            }
        break;

    case FN_RELEASE:
#ifdef DEBUG
        printf("FN_RELEASE %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
        printf("Window position : x = %d: y=%d \n", st_gui.st_animationdata.x,st_gui.st_animationdata.y);
#endif
        /* Start animation*/
        st_gui.st_animationdata.direction = ANIMATION_UP;
        if( st_gui.st_animationdata.is_animation == FALSE ){
            st_gui.st_animationdata.timer_show_id = g_timeout_add(ANIMATION_INTERVAL,
                show_animation, &st_gui.st_animationdata);
        }
        /* Update logic follow user choose */
        switch(st_gui.current_frame){
        case MONITOR_FRAME_TYPE:
            pthread_mutex_lock(&lock);
            pthread_cond_wait(&cv, &lock);
            /* Update logic */
            set_monitor_mode(st_gui.p_fr_monitor->monitor_select);
            pthread_mutex_unlock(&lock);
            /* Update GUI */
            update_window_data();
            break;

        case BRIGHTNESS_FRAME_TYPE:
            break;

        case NETWORK_FRAME_TYPE:
            /* Check the network recovering */
            if (!network_info_s.is_switching && network_info_s.is_recovering) {
                pthread_cancel(thread_update_network);
                network_info_s.is_recovering = FALSE;
            }
            if (!network_info_s.is_switching && !network_info_s.is_recovering) {
                network_info_s.is_switching = TRUE;
                int ptr = st_gui.p_fr_network->network_select;
                pthread_create(&thread_update_network, NULL, &update_network_thread, (void *)&ptr);
            }
            break;

        case TOUCHPAD_FRAME_TYPE:
            set_touchpad_mode(st_gui.p_fr_touchpad->mode_select);
            break;

#ifdef ENABLE_CTRLPANEL
        case CTRLPANEL_FRAME_TYPE:
            break;
#endif
        }

        /* Reset other select */
        gdk_threads_enter();
        /* Update monitor frame */
        update_extend_monitor(st_gui.p_fr_monitor);
        /* Update touch pad frame */
        update_touchpad_icon();
        /* Update network frame */
        update_network_icon();
        /* Update home frame */
        update_home_frame();
        gdk_threads_leave();
        break;

    case FN_SPACE:
        if ( pressed_H == 1 ) {
            detected_fn_space();
        }
        break;

    case FN_F1:
        system(send_logout_event);
        break;

    case FN_ESC:
        system("/usr/bin/sound_toggle");
        break;

    case VGA_NOTIFY:
        /* Update VGA/HDMI plug status */
        update_plugged_status();
        gdk_threads_enter();
        /* Update home frame */
        update_home_frame();
        /* Update monitor frame */
        update_extend_monitor(st_gui.p_fr_monitor);
        gdk_threads_leave();
        /* Update window position */
        update_window_data();
        break;

    case UPDATE_STT_NOTIFY:
        /* Update status */
        get_battery_info();
        /* Update network status */
        update_network_signal();
        /* Update status frame */
        gdk_threads_enter();
        update_status_icon(st_gui.p_fr_status);
        gdk_threads_leave();
        break;

    case NETWORK_NOTIFY:
#ifdef DEBUG_NETWORK
        printf("Network change notify\n");
#endif
        if (!network_info_s.is_switching && network_info_s.is_recovering) {
            pthread_cancel(thread_update_network);
            network_info_s.is_recovering = FALSE;
        }
        /* Trouble network and retries connecting network */
        if (!network_info_s.is_switching && !network_info_s.is_recovering) {
            network_info_s.is_recovering = TRUE;
            pthread_create(&thread_update_network, NULL, &update_network_thread, NULL);
        }
        break;

    case RESUME_NOTIFY:
        set_touchpad_mode(touch_pad_s.current_mode);
        change_brightness_BIOS(brightness_s.current_value);
        break;

    }
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    signal_event_handler
 *
 * Register all event that application need to to handle
 *
 * @param void * thread_arg
 * @n
 *------------------------------------------------------------------*/
void signal_event_handler(void *thread_arg)
// Register all event that application need to to handle
{
    unsigned char readbuff;
    int length = 0;
    char buf_task_ID[10];
    /* Register to monitor VGA event and update status */
    pthread_create (&st_event.p_thread_detect_VGA, NULL, (void *)&monitor_system_status, NULL);
    /* Register to handle keyboard event and network event */
    st_event.sigPlugIn.sa_sigaction = send_notification_event;
    st_event.sigPlugIn.sa_flags = SA_SIGINFO;
    /* Share PID of indicator with tvalz module */
    st_event.config_fd = open(PROCFS_FILE_NAME_PATH, O_RDWR);

    if(st_event.config_fd < 0){
        printf("%s: Failed to open PROC File\n", __FUNCTION__);
    }
    /* Get Indicator PID */
    sprintf(buf_task_ID, "%i", (int)syscall(__NR_gettid));
    printf("The TaskID is %d\r\n", (int)syscall(__NR_gettid));
    /* Write Indicator PID to TVALZ file*/
    if (write(st_event.config_fd, buf_task_ID, strlen(buf_task_ID) + 1) < 0){
        printf("%s: Failed to write PROC File\n", __FUNCTION__);
    }
    /* Set action for signal */
    sigaction(SIG_INDICATOR_SIZE, &st_event.sigPlugIn, NULL);
    /* Monitor TVALZ file*/
    do {
        length = read(st_event.config_fd, &readbuff, 1);
    } while (length == 0 || errno == EINTR);

    printf("The length value is %d\r\n",length);
    printf("closing file now..\r\n");
    close(st_event.config_fd);
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    handle_system_display
 *
 * Create and handle GUI event
 * @n
 *------------------------------------------------------------------*/
void handle_system_display() // Create and handle GUI event
{
    /* Secure gtk */
    gdk_threads_init();
    gdk_threads_enter();
    gtk_init(NULL, NULL);
    init_frame();
    /* If there isn't any network connection, notify the network handler */
    handle_sig_event(NETWORK_NOTIFY);
    gtk_main();
    gdk_threads_leave();
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    update_network_thread
 *
 * Handle for switch/recover network
 *
 * @param void * arg
 * @return void
 *------------------------------------------------------------------*/
static void *update_network_thread(void *arg)
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    int ret = SUCCESS;
    if (arg == NULL) {
        ret = retry_connect_network();
    } else {
        int network_mode = *(int *)arg;
        ret = switch_network(network_mode);
    }
    /* Update all network icons */
    gdk_threads_enter();
    /* Update network frame */
    update_network_icon();
    /* Update home frame */
    update_home_frame();
    gdk_threads_leave();
    network_info_s.is_switching = FALSE;
    network_info_s.is_recovering = FALSE;
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    return NULL;
}
/** **************************************************************************
 *  For Only Test GUI
 ***************************************************************************/
//BATTERY_INFO battery_s;
//BRIGHTNESS_INFO brightness_s;
//MONITOR_INFO monitor_info_s;
//NETWORK_INFO network_info_s;
//TOUCH_PAD_INFO touch_pad_s;
//gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

//int main(int argc, char *argv [])
//{
//    GtkWidget *window;
//    gtk_init(&argc, &argv);
//    memset(&battery_s, 0 , sizeof(BATTERY_INFO));
//    memset(&brightness_s, 0 , sizeof(BRIGHTNESS_INFO));
//    memset(&monitor_info_s, 0 , sizeof(MONITOR_INFO));
//    memset(&network_info_s, 0 , sizeof(NETWORK_INFO));
//    memset(&touch_pad_s, 0 , sizeof(TOUCH_PAD_INFO));
//    brightness_s.current_value = 1;
//    battery_s.battery_index = 20;
//    window = init_frame();
//    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
//    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(on_key_press), NULL);
//
//    gtk_main();
//    return 0;
//}
//************************************
// <B>Function</B>:    on_key_press
// Description:  This function simulation test GUI without use Fn key:
//               Ctrl+2 -> FN_ESC
//               Ctrl+3 -> FN_3
//               Ctrl+4 -> FN_4
//               Ctrl+5 -> FN_F5
//               Ctrl+6 -> FN_F6
//               Ctrl+7 -> FN_F7
//               Ctrl+8 -> FN_F8
//               Ctrl+9 -> FN_F9
//               Ctrl+s -> FN_PRESS
//               Ctrl+h -> FN_RELEASE
//               Ctrl+p -> change batterry charge status
//               Ctrl+b -> change batterry capacity
// Access:    public
// Returns:   gboolean
// Qualifier:
// Parameter: GtkWidget * widget
// Parameter: GdkEventKey * event
// Parameter: gpointer user_data
//************************************
//gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
//{
//    int i;
//    switch (event->keyval)
//    {
//    case GDK_2: /* FN_ESC */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            //handle_sig_event(FN_ESC);
//        }
//        break;
//    case GDK_3: /* FN_F3 */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            //handle_sig_event(FN_F3);
//        }
//        break;
//    case GDK_4: /* FN_F4 */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            //handle_sig_event(FN_F4);
//        }
//        break;
//    case GDK_5: /* FN_F5 Show monitor indicator */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s\n", "ctrl + 5");
//            show_frame(MONITOR_FRAME_TYPE);
//            //handle_sig_event(FN_F5);
//        }
//        break;
//    case GDK_6: /* FN_F6 Show decrease brightness indicator */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s\n", "ctrl + 3");
//            if(brightness_s.current_value > MIN_BRIGHTNESS)
//                brightness_s.current_value --;
//            show_frame(BRIGHTNESS_FRAME_TYPE);
//            //handle_sig_event(FN_F6);
//        }
//        break;
//    case GDK_7: /* FN_F7 Show increase brightness indicator */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s\n", "ctrl + 4");
//            if(brightness_s.current_value < MAX_BRIGHTNESS)
//                brightness_s.current_value ++;
//            show_frame(BRIGHTNESS_FRAME_TYPE);
//            //handle_sig_event(FN_F7);
//        }
//        break;
//
//    case GDK_8: /* FN_F8 Show network indicator */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s\n", "ctrl + 5");
//            show_frame(NETWORK_FRAME_TYPE);
//            //handle_sig_event(FN_F8);
//        }
//        break;
//    case GDK_9: /* FN_F9: Show touch pad indicator */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            handle_sig_event(FN_F9);
//        }
//        break;
//    case GDK_x: /* Quit app */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s\n", "ctrl + x");
//            gtk_main_quit();
//        }
//        break;
//    case GDK_h:/* FN_RELEASE */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s\n", "ctrl + h");
//            st_gui.st_animationdata.direction = ANIMATION_UP;
//            if( st_gui.st_animationdata.is_animation == FALSE ){
//                st_gui.st_animationdata.timer_show_id = g_timeout_add(ANIMATION_INTERVAL,
//                    timerEventShow, &st_gui.st_animationdata);
//            }
//            /* Update all selection value */
//            monitor_info_s.current_mode = st_gui.p_fr_monitor->monitor_select;
//            network_info_s.current_network_mode = st_gui.p_fr_network->network_select;
//            touch_pad_s.current_mode = st_gui.p_fr_touchpad->mode_select;
//            /* Update frame components following monitor data */
//            update_extend_monitor(st_gui.p_fr_monitor);
//            /* Update home frame */
//            update_home_frame();
//            //handle_sig_event(FN_RELEASE);
//        }
//        break;
//
//    case GDK_s:/* FN_PRESS */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            /* Update status frame */
//            update_status_icon(st_gui.p_fr_status);
//            printf("key pressed: %s\n", "ctrl + s");
//            //
//            show_frame(HOME_FRAME_TYPE);
//            st_gui.st_animationdata.direction = ANIMATION_DOWN;
//            if( st_gui.st_animationdata.is_animation == FALSE ){
//                st_gui.st_animationdata.timer_show_id = g_timeout_add(ANIMATION_INTERVAL,
//                    timerEventShow, &st_gui.st_animationdata);
//            }
//            //handle_sig_event(FN_PRESS);
//        }
//        break;
//    case GDK_b: /* Change battery capacity */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s\n", "ctrl + h");
//            battery_s.battery_index+=10;
//        }
//        break;
//    case GDK_p: /* Show/hide charge */
//        if (event->state & GDK_CONTROL_MASK)
//        {
//            printf("key pressed: %s %d \n", "ctrl + 1", battery_s.battery_charge);
//            battery_s.battery_charge = !battery_s.battery_charge;
//        }
//        break;
//    default:
//        return FALSE;
//    }
//    return FALSE;
//}
