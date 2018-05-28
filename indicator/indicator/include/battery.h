#ifndef _BATTERY_
#define _BATTERY_

/** Struct battery*/
typedef struct _BatteryInfo{
    int battery_index;
    int battery_charge;
} BatteryInfo, *PointerBatteryInfo;

/**  Global variable */
extern BatteryInfo battery_s;
/**  Public function */
void get_battery_info();

#endif