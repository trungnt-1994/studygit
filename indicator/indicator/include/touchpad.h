#ifndef _TOUCHPAD_
#define _TOUCHPAD_
/** Enum definition*/
enum Touchpad_mode1 {
    TOUCH_ON,
    TOUCH_OFF
};

/**  Touch pad struct */
typedef struct _TouchPadStruct{
    int     current_mode;
} TouchPadInfo, *PointerTouchPadInfo;

/** Public function*/
int get_touchpad_status();
int set_touchpad_mode(int touchpad);

/** Global variables*/
extern TouchPadInfo touch_pad_s;

#endif
