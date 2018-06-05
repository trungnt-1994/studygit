#ifndef _COMMON_H
# define _COMMON_H

#include <string.h>
#include <time.h>
#include <unistd.h>

/* count number of elements of an array, it should be an compile error if passing x as a pointer */
#define COUNTOF(x) ((sizeof (x) / sizeof (0[x])) / ((size_t) !(sizeof (x) % sizeof(0[x]))))

/* get date and time as format */
#define FPRINT_DATETIME(x, f) \
    do { \
        time_t now; time(&now); \
        strftime(x, COUNTOF((x)), (f), localtime(&now)); \
    } while(0)

enum {SYNAPTIC, ALPS};

#ifdef __MAIN__
int touchpad_type;   /* type of machine touchpad */

#define SYNAPTIC_CONF "/etc/syntp_sensitive.conf"   /* path to synaptic sensitive conf file */

__attribute__((constructor))
static void touchpad_detector(void)
{
    touchpad_type = (access(SYNAPTIC_CONF, F_OK) == 0) ? SYNAPTIC : ALPS;
}
# else
extern int touchpad_type;
#endif

#endif
