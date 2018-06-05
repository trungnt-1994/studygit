#ifndef _HELPER_
#define _HELPER_
#include "tzcslib/TzcsUtil.h"
/** ***************************************************
*                    MACRO DEFINITION                     *
*****************************************************/
#define TZCS_DEBUG                  1
#define _EXIT_ERROR_                0
#define _EXIT_OK_                   1
#define _NETWORK_CHAGING_STATUS_    2
#define _NETWORK_WIRE_MODE_         3
#define _NETWORK_WIFI_MODE_         4
#define _NETWORK_DISCONNECTED_      5

/** Time interval definition*/
#define KEY_INTERVAL                40000
#define IDLE_INTERVAL               10000
#define NETWORK_INTERVAL            6000000

/** Device code definition*/
#define ALTAIR_UX20_MX20            0x9E
#define ALTAIR_LE_PLUS              0xA5
#define ALTAIR_SE30_LE30            0xAB

/** Debug code definition*/
#define FN_PRESS                    0x15e
#define FN_RELEASE                  0x15f
#define FN_F1                       0x13b
#define FN_F2                       0x13c
#define FN_F3                       0x104
#define FN_F4                       0x105
#define FN_F5                       0x13f
#define FN_F6                       0x140
#define FN_F7                       0x141
#define FN_F8                       0x142
#define FN_F9                       0x143
#define FN_SPACE                    0x139
#define FN_ESC                      0x101
#define NETWORK_NOTIFY              1235
#define VGA_NOTIFY                  1236
#define UPDATE_STT_NOTIFY           1237
#define SUSPEND_NOTIFY              1238
#define RESUME_NOTIFY               1239

/**  Define return value */
#define OK                          0
#define NG                          -1
/**  Define BIOS access */
#define BIOS_TOUCHPAD               0x005f
#define BIOS_BRIGHTNESS             0x002a
#define BIOS_READ                   0xfe00
#define BIOS_WRITE                  0xff00
/**  File system definition*/
extern char *TVALZ0_FILE_SYSTEM;

#if HAVE_GETTEXT
#define _(String) gettext(String)
#else
#define _(String) String
#endif

#undef TRUE
#undef FALSE
#define FALSE 0
#define TRUE  1

int init_hardware_driver();
char *trim(char *str);
char *get_status(const char *cmd);
int get_info_val(const char *cmd);
void get_client_status_kill();
int acces_BIOS_value(int access_mode, int object_type, ULONG* rev_value, ULONG mod_value);

/** ******************************************************************************
 * Monitor object
 ******************************************************************************/

#endif
