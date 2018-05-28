#include <string.h>
#include <stdlib.h>
#include "battery.h"
#include <stdio.h>
#include "helper.h"
/**  Get battery status(Charge or not)*/
static const char *checkbatterycharge    = "cat /proc/acpi/ac_adapter/ADP1/state | awk '{print $2}'"; 
static const char *getcurrentbattery     = "cat /proc/acpi/battery/BAT1/state | awk '{if (NR==5) {print $3}}'";
static const char *getfullbattery        = "cat /proc/acpi/battery/BAT1/info | awk '{if(NR==3) {print $4}}'";
static char *status_on = "on-line";
/**  Global variable */
BatteryInfo battery_s={};

/**-------------------------------------------------------------------
 * <B>Function</B>:get_battery_info
 * 
 * Get information about battery capacity and charger status
 *
 * @n
 *------------------------------------------------------------------*/
void get_battery_info() // Get information about battery capacity and charger status
{
    /*get battery capacity */
    battery_s.battery_index = (get_info_val(getcurrentbattery) * 100)/get_info_val(getfullbattery);
    /*get battery charge */
    char *battery_status_temp = get_status(checkbatterycharge);
    if (strcmp(trim(battery_status_temp),status_on) == 0 ) {
        battery_s.battery_charge = TRUE;
    }
    else {
        battery_s.battery_charge = FALSE;
    }
#ifdef DEBUG_BATTERY
    printf("Value bat_index = %d: bat_charge = %d \n", battery_s.battery_index, battery_s.battery_charge);
#endif // DEBUG_BATTERY
    /* Free memory */
    free(battery_status_temp);
}
