#include "brightness.h"
#include "tvalzctl.h"
#include "helper.h"

/**  Declare global variable */
BrightnessInfo brightness_s={0};

int change_brightness_BIOS(int change_mount);
int get_BIOS_brightness(int *brightness);

/**-------------------------------------------------------------------
 * <B>Function</B>:set_current_brightness
 * 
 * Set current brightness to BIOS
 *
 * @n
 *------------------------------------------------------------------*/
void set_current_brightness() //Set current brightness to BIOS
{
    change_brightness_BIOS(0);
}


/**-------------------------------------------------------------------
 * <B>Function</B>:get_brightness
 * 
 * Convert brightness value from BIOS to brightness level
 *
 * @n
 *------------------------------------------------------------------*/
void get_brightness() // Convert brightness value from BIOS to brightness level
{
    int return_status;
    int brightness_index = MAX_BRIGHTNESS;
    return_status = get_BIOS_brightness(&brightness_index);
    if (return_status == OK)
    {
        // Max 8 level as fixed
        int brightness_step = MAX_BRIGHTNESS / 8;
        int level = 0;
        level = brightness_index/brightness_step;
        brightness_s.current_value = level;
    }else{
        /* Error happen */
    }
}

 /**-------------------------------------------------------------------
 * <B>Function</B>:increase_brightness_BIOS
 * 
 * Increase brightness more than one level
 *
 * @n
 *------------------------------------------------------------------*/
void increase_brightness_BIOS()// Increase brightness more than one level
{
    int return_status = OK;
    if(brightness_s.current_value < MAX_BRIGHTNESS){
        return_status = change_brightness_BIOS(1);
        if (return_status == OK){
            brightness_s.current_value = brightness_s.current_value +1;
        }
    }
}


/**-------------------------------------------------------------------
 * <B>Function</B>:descrease_brightness_BIOS
 * 
 * Decrease brightness more than one level
 *
 * @n
 *------------------------------------------------------------------*/
void descrease_brightness_BIOS()// Decrease brightness more than one level
{
    int return_status = OK;
    if(brightness_s.current_value > MIN_BRIGHTNESS){
        return_status = change_brightness_BIOS(-1);
        if (return_status == OK){
            brightness_s.current_value = brightness_s.current_value -1;
        }
    }
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    change_brightness_BIOS
 * 
 * Change brightness level in BIOS
 *
 * @return  Return OK in normal case or error code if an error occurred
 *
 * @param int change_mount
 * @n
 *------------------------------------------------------------------*/
int change_brightness_BIOS(int change_mount)// Change brightness level in BIOS
{
    int return_status = OK;
    /* Set value */
    ULONG ul_brightness = (brightness_s.current_value -1 + change_mount) << 13;
    /* Set current brightness to BIOS*/
    return_status = acces_BIOS_value(BIOS_WRITE, BIOS_BRIGHTNESS, &ul_brightness, ul_brightness);
    return return_status;
}

/**-------------------------------------------------------------------
 * <B>Function</B>:    get_BIOS_brightness
 * 
 * Get brightness level from BIOS
 *
 * @return  Return OK in normal case or error code if an error occurred
 * 
 *------------------------------------------------------------------*/
int get_BIOS_brightness(int *brightness)// Get brightness level from BIOS
{
    int return_status = OK;
    ULONG ul_value;
    /* Check if this pc supports touch pad driver*/
    return_status = acces_BIOS_value(BIOS_READ, BIOS_BRIGHTNESS, &ul_value, 0);
    if (return_status == OK){
        /* Set value*/
        *brightness = (ul_value >> 13) + 0x1;
    }
    return return_status;
}
