/** ****************************************/
/**  Author : TSDV                         */
/** ****************************************/
#ifndef __HDMI_CERTIFICATE__
#define __HDMI_CERTIFICATE__

void exec_command_line(int pos);
void detected_fn_space();
char *get_status_hdmi(const char *cmd);
void get_key_pressed_BIOS(unsigned int key);

/**  Keycode definition */
#define H_CODE 0x29
#define Fn_Space_CODE 0x139

extern int pressed_H;

#endif
