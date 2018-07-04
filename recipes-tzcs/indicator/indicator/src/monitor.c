#include "monitor.h"
#include "helper.h"
#include "brightness.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**  Declare functions */
void update_plugged_status(void);
int  get_screen_plugged_action(void);
void make_fullscreen(int monitor_mode);
void execute_loading(void);

/**  Declare variable */
MonitorInfo monitor_info_s ={0};

/**-------------------------------------------------------------------
 * <B>Function</B>:    init_monitor_status
 * 
 * Init monitor status
 *
 *------------------------------------------------------------------*/
void init_monitor_status()// Init monitor status
{
    monitor_info_s.current_mode = INTERNAL;
    monitor_info_s.is_switching = FALSE;
    monitor_info_s.first_init_time = 1;
    monitor_info_s.un_plugVGA = TRUE;
    monitor_info_s.un_plugHDMI = TRUE;
    monitor_info_s.current_output = INTERNAL_SCREEN;
    update_plugged_status();
};

/**-------------------------------------------------------------------
 * <B>Function</B>:    get_screen_plugged_status
 * 
 * Get VGA/HDMI status
 *
 *------------------------------------------------------------------*/
int get_screen_plugged_action()// Get VGA/HDMI status
{
    int _un_plugVGA = TRUE;
    int _un_plugHDMI = TRUE;
    int ret = PLUG_IN;
    char *vga_status = trim(get_status(CHECK_PLUG_VGA));
    char *hdmi_status = trim(get_status(CHECK_PLUG_HDMI));
    if (strcmp("connected", vga_status) == 0 ) {
        _un_plugVGA = FALSE;
    }
    if (strcmp("connected", hdmi_status) == 0) {
        _un_plugHDMI = FALSE;
    }
    /** Check that action is plug or unplug */
    if ((monitor_info_s.un_plugHDMI + monitor_info_s.un_plugVGA) < (_un_plugVGA + _un_plugHDMI)){
        ret = UN_PLUG;
    }
    if ((!monitor_info_s.un_plugHDMI)&&(!_un_plugHDMI)){ /** Keep HDMI plug-in, change VGA state*/
        if (monitor_info_s.current_output == HDMI){
            ret = NO_ACTION;
        }
    }
    if ((!monitor_info_s.un_plugVGA)&&(!_un_plugVGA)){ /** Keep VGA plug-in, change HDMI state*/
        if (monitor_info_s.current_output == VGA){
            ret = NO_ACTION;
        }
    }
    monitor_info_s.un_plugVGA = _un_plugVGA;
    monitor_info_s.un_plugHDMI = _un_plugHDMI;
    return ret;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    update_plugged_status
 * 
 * Set monitor status base on VGA/HDMI status
 *
 *------------------------------------------------------------------*/
void update_plugged_status()// Set monitor status base on VGA/HDMI status
{
    int action;
    /* Get screen status change */
    action = get_screen_plugged_action();
    /** Handle action */
    if (action == PLUG_IN){
#ifdef DEBUG_MONITOR
        printf("Plug in VGA/HDMI \n");
        printf("Monitor current output =%d, current mode = %d \n", monitor_info_s.current_output, monitor_info_s.current_mode);
#endif // DEBUG
        system(EX_INTERNAL);
        if (monitor_info_s.first_init_time) {
            system("\
                    xrandr --auto\n\
                    xrandr --output DP1 --off\n\
                    xrandr --output DP2 --off\n\
                    xrandr --output HDMI1 --off\n\
                    xrandr --output HDMI2 --off\n\
                    ");
            system("hdmi-sound-switch 0 &");
            monitor_info_s.first_init_time = 0;
        }
    } else if (action == UN_PLUG){
#ifdef DEBUG_MONITOR
        printf("Unplug VGA/HDMI \n");
        printf("Monitor current output =%d, current mode = %d \n", monitor_info_s.current_output, monitor_info_s.current_mode);
#endif // DEBUG
        system(EX_INTERNAL);
        execute_loading();
        /* Turn off external monitor */
        system("\
               xrandr --auto\n\
               xrandr --output DP1 --off\n\
               xrandr --output DP2 --off\n\
               xrandr --output HDMI1 --off\n\
               xrandr --output HDMI2 --off\n\
               ");
        system("hdmi-sound-switch 0 &");
        /* Reset mode */
        if (monitor_info_s.current_mode > INTERNAL) {
            monitor_info_s.current_mode = INTERNAL;
        }
        monitor_info_s.is_switching = FALSE;
        monitor_info_s.current_output = INTERNAL_SCREEN;
        set_current_brightness();
        make_fullscreen(monitor_info_s.current_mode);
    }
}


/**-------------------------------------------------------------------
 * <B>Function</B>:    set_monitor_mode
 * 
 * Set monitor mode to  select mode
 *
 * @param int select_mode
 * 
 *------------------------------------------------------------------*/
void set_monitor_mode(int select_mode)// Set monitor mode to new select mode
{
    if ((monitor_info_s.current_mode != select_mode)
        || (monitor_info_s.current_mode == SWITCH)) {
        /** Update current output */
        if (!monitor_info_s.un_plugHDMI) {
            monitor_info_s.current_output = HDMI;
        } else {
            monitor_info_s.current_output = VGA;
        }
        /** Update current mode */
        switch (select_mode) {
        case SWITCH:
            system(EX_SWITCH);
            execute_loading();
            monitor_info_s.current_mode = SWITCH;
            monitor_info_s.is_switching = !monitor_info_s.is_switching;
            break;
        case INTERNAL:
            system(EX_INTERNAL);
            execute_loading();
            monitor_info_s.current_mode = INTERNAL;
            monitor_info_s.is_switching = FALSE;
            break;
        case EXTERNAL:
            system(EX_EXTERNAL);
            execute_loading();
            monitor_info_s.current_mode = EXTERNAL;
            monitor_info_s.is_switching = TRUE;
            break;
        case CLONE:
            if ((monitor_info_s.current_mode == SWITCH)||(monitor_info_s.current_mode == EXTERNAL)){
                system(EX_INTERNAL);
            }
            system(EX_CLONE);
            execute_loading();
            monitor_info_s.current_mode = CLONE;
            break;
        case EXTEND:
            if ((monitor_info_s.current_mode == CLONE)||(monitor_info_s.current_mode == EXTERNAL)){
                system(EX_INTERNAL);
            }
            system(EX_EXTEND);
            execute_loading();
            monitor_info_s.current_mode = EXTEND;
            monitor_info_s.is_switching = FALSE;
            break;
        default:
            break;
        }
        set_current_brightness();
        make_fullscreen(monitor_info_s.current_mode);
        /* Switch sound if need */
        if (monitor_info_s.current_mode == INTERNAL) {
            system("hdmi-sound-switch 0 &");
        } else {
            if ((monitor_info_s.current_output == HDMI)&&(monitor_info_s.current_mode != INTERNAL)) {
                system("hdmi-sound-switch 1 &");
            }
        }
#ifdef DEBUG_MONITOR
        printf(" Monitor: current output(0:INTER|1:VGA|2:HDMI): %d\n", monitor_info_s.current_output);
        printf(" Monitor: current mode (0:INTERNAL|1:EXTERNAL|2:CLONE|3:EXTEND|4:SWITCH): %d\n", monitor_info_s.current_mode);
#endif // DEBUG_MONITOR
    } 
}

/**-------------------------------------------------------------------
 * <B>Function</B>:execute_loading
 *
 * This function calls shell script to execute loading animation
 *
 * @returns   void
 *
 * @n
 *------------------------------------------------------------------*/
void execute_loading()
{
#ifdef MAKE_FULLSCREEN
    system("loading &");
#endif
}

/**-------------------------------------------------------------------
 * <B>Function</B>:make_fullscreen
 *
 * This function calls shell script which fake mouse and keyboard actions
 * to choose Fullscreen mode for VDI
 *
 * @returns   void
 *
 * @n
 *------------------------------------------------------------------*/
void make_fullscreen(int monitor_mode)
{
#ifdef MAKE_FULLSCREEN
    if (monitor_mode == EXTEND || monitor_mode == SWITCH) {
        system("make-fullscreen all &");
    } else {
        system("make-fullscreen &");
    }
#endif
}
/**-------------------------------------------------------------------
 * <B>Function</B>:    get_current_profile
 * 
 * Get monitor mode from xrandr
 *@n
 *------------------------------------------------------------------*/
profile_t get_current_profile()// Synchronize monitor status
{
    printf ("===> Enter function %s, %d\n",__FUNCTION__, __LINE__);
    profile_t profile;
    printf("Get profile command: %s\n", GET_CURRENT_PROFILE);
    char *current_profile = trim(get_status(GET_CURRENT_PROFILE)); 
    printf("current profile: %s\n", current_profile);
    /* Check monitor mode */
    if(strstr(current_profile, "single") != NULL) {
        profile = profile_single;
    }
    if(strstr(current_profile, "external") != NULL) {
        profile = profile_external;
    }
    if(strstr(current_profile, "clone") != NULL) {
        profile = profile_clone;
    }
    if(strstr(current_profile, "extend right") != NULL) {
        profile = profile_extend_right;
    }
    if(strstr(current_profile, "extend below") != NULL) {
        profile = profile_extend_below;
    }
    if(strstr(current_profile, "extend left") != NULL) {
        profile = profile_extend_left;
    }
    if(strstr(current_profile, "extend above") != NULL) {
        profile = profile_extend_above;
    }
    if(strstr(current_profile, "switch right") != NULL) {
        profile = profile_switch_right;
    }
    if(strstr(current_profile, "switch left") != NULL) {
        profile = profile_switch_left;
    }
    if(strstr(current_profile, "switch below") != NULL) {
        profile = profile_switch_below;
    }
    if(strstr(current_profile, "switch above") != NULL) {
        profile = profile_switch_above;
    }
    return profile;
};
/**-------------------------------------------------------------------
 * <B>Function</B>:    synchronize_monitor_status
 * 
 * Synchronize monitor status
 * @n
 *------------------------------------------------------------------*/
int synchronize_monitor_status()// Synchronize monitor status
{
#ifdef DEBUG_MONITOR
    printf ("===> Enter function %s, %d\n",__FUNCTION__, __LINE__);
#endif
    int bReturn = 0;
    profile_t profile = get_current_profile();
    monitor_info_s.extend_type = profile;
    /* Synchronize monitor mode */
    switch (profile){
    case profile_single:
        if (monitor_info_s.current_mode != INTERNAL){
            monitor_info_s.current_mode = INTERNAL;
            bReturn = 1;
        };
        break;
    case profile_external:
        if (monitor_info_s.current_mode != EXTERNAL){
            monitor_info_s.current_mode = EXTERNAL;
            bReturn = 1;
        };
        break;
    case profile_clone:
        if (monitor_info_s.current_mode != CLONE){
            monitor_info_s.current_mode = CLONE;
            bReturn = 1;
        };
        break;
    case profile_extend_right:
        if (monitor_info_s.current_mode != EXTEND){
            monitor_info_s.current_mode = EXTEND;
            bReturn = 1;
        };
        break;
    case profile_switch_right:
    case profile_extend_below:
    case profile_extend_left:
    case profile_extend_above:
    case profile_switch_below:
    case profile_switch_left:
    case profile_switch_above:
        if (monitor_info_s.current_mode != SWITCH){
            monitor_info_s.current_mode = SWITCH;
            bReturn = 1;
        };
        break;
    default:
        break;
    }
#ifdef DEBUG_MONITOR
    printf ("<=== Exit function %s, %d\n",__FUNCTION__, __LINE__);
#endif
    return bReturn;
};

/**-------------------------------------------------------------------
 * <B>Function</B>:    get_screen_info
 * 
 * Get all out put screen
 *@n
 *------------------------------------------------------------------*/
pOutputs get_screen_info ()
{
    pOutputs outputs = NULL;
    Display *display_screen = NULL;
    XRRScreenResources *res = NULL;
    RROutput primary_id;
    int i, j;
#ifdef DEBUG_MONITOR
    printf("Enter to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    display_screen = XOpenDisplay(NULL);
    if (display_screen == NULL){
        printf("XOpenDiplay return NULL \n");
    } else{
        res = XRRGetScreenResources(display_screen, DefaultRootWindow(display_screen));
        if (res == NULL){
            printf("XRRGetScreenResources return NULL \n");
        } else{
            int number_screen = 0;
            printf("Count number of output \n");
            for (i = 0; i < res->noutput; i++)
            {
                XRROutputInfo *output_info = XRRGetOutputInfo (display_screen, res, res->outputs[i]);
                if (!output_info) {
                    printf ("could not get output 0x%d information\n", res->outputs[i]); 
                    return outputs;
                }
                if (!output_info->connection) number_screen++;
                XRRFreeOutputInfo(output_info);
            }
            /* Allocate memory for outputs */
            outputs = (pOutputs)malloc(sizeof(Outputs));
            memset(outputs, 0, sizeof(Outputs));
            outputs->nscreen = number_screen;
            outputs->nprimary = -1;
            outputs->out_screens = calloc (number_screen, sizeof (OutputScreen));
            memset(outputs->out_screens, 0, number_screen*sizeof (OutputScreen));
            int scr_index = 0;
            primary_id = XRRGetOutputPrimary(display_screen, DefaultRootWindow(display_screen));
            for (i = 0; i < res->noutput; i++)
            {
                XRROutputInfo *output_info = XRRGetOutputInfo (display_screen, res, res->outputs[i]);
                if (!output_info->connection){
                    XRRCrtcInfo *crtc_info = NULL;
                    /* Get name of screen */
                    outputs->out_screens[scr_index].name = strdup(output_info->name);
                    /* Get size of screen */
                    for (j = 0; j < res->ncrtc;j++){
                        crtc_info = XRRGetCrtcInfo(display_screen, res, res->crtcs[j]);
                        if (crtc_info == NULL){
                            continue;;
                        }
                        else{
                            if (crtc_info->noutput > 0){
                                if(crtc_info->outputs[crtc_info->noutput - 1] == res->outputs[i]){
                                    break;
                                }
                            }
                            XRRFreeCrtcInfo(crtc_info);
                        }
                    }
                    if (crtc_info == NULL){
                        printf("Error get crtc info \n");
                    }
                    else{
                        outputs->out_screens[scr_index].width = crtc_info->width;
                        outputs->out_screens[scr_index].height = crtc_info->height;
                        XRRFreeCrtcInfo(crtc_info);
                    }
                    /* Set internal position screen */
                    if(!strcmp(output_info->name,"eDP1"))
                    {
                        outputs->ninternal = scr_index;
                    }else{
                        outputs->nsecond = scr_index;

                    }
                    /* Set primary position screen */
                    if (res->outputs[i] == primary_id){
                        printf("Primary monitor name: %s\n", outputs->out_screens[scr_index].name);
                        outputs->nprimary = scr_index;
                    }
                    scr_index++;
                }
                XRRFreeOutputInfo(output_info);
            }
        }
    }
#ifdef DEBUG_MONITOR
    for (i = 0; i < outputs->nscreen; i++)
    {
        printf("==== Monitor %d =========\n", i);
        printf("Name: %s, Primary: %s \n", p_outputs->out_screens[i].name, p_outputs->out_screens[i].is_primary==1?"Yes":"No");
        printf("width %d, height %d \n", p_outputs->out_screens[i].width, p_outputs->out_screens[i].height);
    }
    printf("Exit to %d: %s:%s \n",  __LINE__, __FUNCTION__,__FILE__);
#endif
    return outputs;
}
