#ifndef _BRIGHTNESS_
#define _BRIGHTNESS_
/**  Define constant */
#define MAX_BRIGHTNESS      0x8
#define MIN_BRIGHTNESS      0x1



/**  Brightness struct */
typedef struct _BrightnessInfo{
    int     current_value;
} BrightnessInfo, *PointerBrightnessInfo;
/**  Global variable */
extern BrightnessInfo brightness_s;
/**  Public function */
void get_brightness();
void set_current_brightness();
void increase_brightness_BIOS();
void descrease_brightness_BIOS();



#endif
