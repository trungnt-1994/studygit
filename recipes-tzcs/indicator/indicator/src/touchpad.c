#include "helper.h"
#include "touchpad.h"
#include <sys/types.h>
#include <sys/wait.h>

TouchPadInfo touch_pad_s={};

/**-------------------------------------------------------------------
 * <B>Function</B>:    get_touchpad_status
 *
 * Get touchpad status from BIOS
 *
 * @return        OK : successful
 * @return        NG : Some error happen
 *
 * @n
 *------------------------------------------------------------------*/
int get_touchpad_status()
// Get touchpad status from BIOS
{
#if 0    //haya
    int return_status = OK;
    ULONG ul_value;
    /*check if this pc supports touchpad driver*/
    return_status = acces_BIOS_value(BIOS_READ, BIOS_TOUCHPAD, &ul_value, 0);
    if (return_status == OK){
        /* Set value*/
        if(ul_value == 1){
            touch_pad_s.current_mode = TOUCH_ON;
        }else{
            touch_pad_s.current_mode = TOUCH_OFF;
        }
    }
    return return_status;
#else    //haya
    int return_status = OK;
    ULONG ul_value;
    int ret;
    int type_touchpad;

    ret = system("grep AlpsPS /var/log/SelectTouchPadName > /dev/null 2>&1");

    if(WIFEXITED(ret)){
        type_touchpad = WEXITSTATUS(ret);
        if( type_touchpad == 0 ) {
            g_print ("Select Alps TouchPad (SelectTouchPadName) \n");
            type_touchpad = 0;
        }else{
            g_print ("Select Synaptics TouchPad (SelectTouchPadName) \n");
            type_touchpad = 1;
        }
    }else{
        type_touchpad = 0xFF;
        g_print ("Command : grep SelectTouchPadName - NG (Select Synaptics TouchPad) \n");
    }

    /*check if this pc supports touchpad driver*/
    return_status = acces_BIOS_value(BIOS_READ, BIOS_TOUCHPAD, &ul_value, 0);

    if (type_touchpad != 0){
        /************************************/
        /* Synaptics & Other Touch Pad      */
        /************************************/
        if (return_status == OK){
            /* Set value*/
            if(ul_value == 1){
                touch_pad_s.current_mode = TOUCH_ON;
                g_print ("Synaptics TouchPad (TOUCH_ON) \n");
            }else{
                touch_pad_s.current_mode = TOUCH_OFF;
                g_print ("Synaptics TouchPad (TOUCH_OFF) \n");
            }
        }
    }else{
        /************************************/
        /* ALPS Touch Pad                   */
        /************************************/
        ret = system("psmouse_tos getdev | grep 3 > /dev/null 2>&1");

        if(WIFEXITED(ret)){
            if(WEXITSTATUS(ret) == 0) {
                touch_pad_s.current_mode = TOUCH_ON;
                g_print ("ALPS TouchPad (TOUCH_ON) \n");
            }else{
                touch_pad_s.current_mode = TOUCH_OFF;
                g_print ("ALPS TouchPad (TOUCH_OFF) \n");
            }
        }else{
            touch_pad_s.current_mode = TOUCH_ON;
            g_print ("ALPS TouchPad (Dummy TOUCH_ON) \n");
        }
    }

    return return_status;
#endif    //haya
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    set_touchpad_mode
 *
 * Set touchpad mode to BIOS
 *
 * @return        OK : successful
 * @return        NG : Some error happen
 *
 * @param int touchpad
 * @n
 *------------------------------------------------------------------*/
int set_touchpad_mode(int touchpad)
// Set touchpad mode to BIOS
{
#if 0    //haya
    int return_status = OK;
    ULONG ul_value;
    /*Set value to BIOS*/
    return_status = acces_BIOS_value(BIOS_WRITE, BIOS_TOUCHPAD, &ul_value, (ULONG)(1 - touchpad));
    if (return_status != OK){
        /* handle error */
    }else{
        /* Update touchpad status */
        touch_pad_s.current_mode = touchpad;
    }
    return return_status;
#else    //haya
    int return_status = OK;
    ULONG ul_value;
    int ret;
    int type_touchpad;

    ret = system("grep AlpsPS /var/log/SelectTouchPadName > /dev/null 2>&1");

    if(WIFEXITED(ret)){
        type_touchpad = WEXITSTATUS(ret);
        if( type_touchpad == 0 ) {
            g_print ("Select Alps TouchPad (SelectTouchPadName) \n");
            type_touchpad = 0;
        }else{
            g_print ("Select Synaptics TouchPad (SelectTouchPadName) \n");
            type_touchpad = 1;
        }
    }else{
        type_touchpad = 0xFF;
        g_print ("Command : grep SelectTouchPadName - NG (Select Synaptics TouchPad) \n");
    }

    if (type_touchpad != 0){
        /************************************/
        /* Synaptics & Other Touch Pad      */
        /************************************/
        /*Set value to BIOS*/
        return_status = acces_BIOS_value(BIOS_WRITE, BIOS_TOUCHPAD, &ul_value, (ULONG)(1 - touchpad));
        if (return_status != OK){
            /* handle error */
        }else{
            /* Update touchpad status */
            touch_pad_s.current_mode = touchpad;
            g_print ("Synaptics TouchPad (%d) \n", touchpad);
        }
    }else{
        /************************************/
        /* ALPS Touch Pad                   */
        /************************************/
        if (touchpad == TOUCH_ON) {
            /* Set TouchPad and Pointing stick Enable by psmouse_tos Command */
            system("psmouse_tos disdev > /dev/null 2>&1");
            g_print ("ALPS TouchPad (Enable) \n");
        }else {                     /* TOUCH_OFF */
            /* Set TouchPad Only Disable by psmouse_tos Command */
            system("psmouse_tos disdev t > /dev/null 2>&1");
            g_print ("ALPS TouchPad (disable) \n");
        }
        /* Update touchpad status */
        touch_pad_s.current_mode = touchpad;
    }

    return return_status;
#endif    //haya
}
