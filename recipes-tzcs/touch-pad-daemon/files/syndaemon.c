/*
 * Copyright © 2003-2004 Peter Osterlund
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Red Hat
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  Red
 * Hat makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors:
 *      Peter Osterlund (petero2@telia.com)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/Xlib.h>
#include <X11/Xatom.h>
//del #include <X11/extensions/XInput.h>
#if 0	// add
#ifdef HAVE_X11_EXTENSIONS_RECORD_H
#include <X11/Xproto.h>
#include <X11/extensions/record.h>
#endif                          /* HAVE_X11_EXTENSIONS_RECORD_H */
#endif	// add

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <sys/ioctl.h>  // add
#include <fcntl.h>      // add
#include "tvalzctl.h"   // add

//del #include "synaptics-properties.h"

enum TouchpadState {
    TouchpadOn = 0,
    TouchpadOff = 1,
//del TappingOff = 2
};

static Bool		pad_disabled
/* internal flag, this does not correspond to device state */ ;
static int		ignore_modifier_combos;
static int		ignore_modifier_keys;
static int		background;
static const	char *pid_file;
static Display	*display;
//del debug static XDevice *dev;
int				dev;			    //add
static Atom		touchpad_off_prop;
static enum		TouchpadState previous_state;
static enum		TouchpadState disable_state = TouchpadOff;
static int		verbose;

static int		type_touchpad;		//add


#define KEYMAP_SIZE 32
static unsigned char keyboard_mask[KEYMAP_SIZE];

static void
usage(void)
{
    fprintf(stderr,
            "Usage: syndaemon [-i idle-time] [-m poll-delay] [-d] [-k]\n");
//          "Usage: syndaemon [-i idle-time] [-m poll-delay] [-d] [-t] [-k]\n");
    fprintf(stderr,
            "  -i How many seconds to wait after the last key press before\n");
    fprintf(stderr, "     enabling the touchpad. (default is 1.0s)\n");
//  fprintf(stderr, "     enabling the touchpad. (default is 2.0s)\n");
    fprintf(stderr, "  -m How many milli-seconds to wait until next poll.\n");
    fprintf(stderr, "     (default is 200ms)\n");
    fprintf(stderr, "  -d Start as a daemon, i.e. in the background.\n");
    fprintf(stderr, "  -p Create a pid file with the specified name.\n");
//  fprintf(stderr,
//                  "  -t Only disable tapping and scrolling, not mouse movements.\n");
    fprintf(stderr,
            "  -k Ignore modifier keys when monitoring keyboard activity.\n");
    fprintf(stderr, "  -K Like -k but also ignore Modifier+Key combos.\n");
//  fprintf(stderr, "  -R Use the XRecord extension.\n");
    fprintf(stderr, "  -v Print diagnostic messages.\n");
    fprintf(stderr, "  -? Show this help message.\n");
    exit(1);
}
#if 1	// add -->
static void
touchpad_type(void)
{
	int ret_status;
	int ret;
	int temp_type;

	ret = system("grep AlpsPS /var/log/SelectTouchPadName > /dev/null 2>&1");

	if(WIFEXITED(ret)){
		temp_type = WEXITSTATUS(ret);

		if( temp_type == 0 ) {
			g_print ("Select Alps TouchPad (SelectTouchPadName) \n");
			type_touchpad = 0x00;
		}else{
			g_print ("Select Synaptics TouchPad (SelectTouchPadName) \n");
			type_touchpad = 0x01;
		}
	}else{
		g_print ("Command : grep SelectTouchPadName - NG (Select Synaptics TouchPad) \n");
		type_touchpad = 0x01;
	}
}

static int
current_touchpad_state(void)
{
	int					ret_status = TouchpadOff;
	int					ret;
	int					temp_type;
	int					fd_touchpad;
	ghci_interface		ghci_buf_touchpad;

	fd_touchpad = open("/dev/tvalz0", O_RDONLY);
	if (fd_touchpad < 0){
	}else{
		// Read hci
		ghci_buf_touchpad.ghci_eax = 0xfe00;		// HCI Get Function
		ghci_buf_touchpad.ghci_ebx = 0x005f;		// Device Output Status
		ghci_buf_touchpad.ghci_ecx = 0x00;			// Touch Pad
		ghci_buf_touchpad.ghci_edx = 0x00;
		ghci_buf_touchpad.ghci_esi = 0x00;
		ghci_buf_touchpad.ghci_edi = 0x00;
		ioctl(fd_touchpad, IOCTL_TVALZ_GHCI,  &ghci_buf_touchpad);
		close(fd_touchpad);
	}

	if(ghci_buf_touchpad.ghci_ecx == 1){
		// TouchPad ON
		ret_status = TouchpadOn;
	}else{
		// TouchPad OFF
		ret_status = TouchpadOff;
	}

	/* ALPS Touch Pad ?		*/
	if( type_touchpad == 0 ) {
		/************************************/
		/* ALPS Touch Pad					*/
		/************************************/
		if( ret_status == TouchpadOff )
			return (ret_status);

		ret = system("psmouse_tos getdev | grep 'getSelectDev : 3'  > /dev/null 2>&1");
		if(WIFEXITED(ret)){
			temp_type = WEXITSTATUS(ret);
			if( temp_type == 0 ) {
				// TouchPad ON
				ret_status = TouchpadOn;
			}else{
				// TouchPad OFF
				ret_status = TouchpadOff;
			}
		}else{
			// TouchPad OFF
			ret_status = TouchpadOff;
		}
		return (ret_status);
	} else {
		/************************************/
		/* Synaptics & Other Touch Pad		*/
		/************************************/
		return (ret_status);
	}
	return (ret_status);
}
#endif	// <-- add


static void
store_current_touchpad_state(void)
{
#if 1	// add
{
	previous_state = current_touchpad_state();
}
#else	// <--add, del-->
    Atom real_type;
    int real_format;
    unsigned long nitems, bytes_after;
    unsigned char *data;

    if ((XGetDeviceProperty(display, dev, touchpad_off_prop, 0, 1, False,
                            XA_INTEGER, &real_type, &real_format, &nitems,
                            &bytes_after, &data) == Success) &&
        (real_type != None)) {
        previous_state = data[0];
    }
#endif	// <--del
}

/**
 * Toggle touchpad enabled/disabled state, decided by value.
 */
static void
toggle_touchpad(Bool enable)
{
    unsigned char data;

    if (pad_disabled && enable) {
        data = previous_state;
        pad_disabled = False;
        if (verbose)
            printf("Enable\n");
    }
    else if (!pad_disabled && !enable &&
             previous_state != disable_state && previous_state != TouchpadOff) {
        store_current_touchpad_state();
        pad_disabled = True;
        data = disable_state;
        if (verbose)
            printf("Disable\n");
    }
    else
        return;

#if 1	// add-->
{
	if( data == TouchpadOn )
		system("Touchpad_ctrl enable > /dev/null 2>&1");		// ON (Enable)
	else
		system("Touchpad_ctrl disable > /dev/null 2>&1");	/// OFF (Disable)
}
#else	// <--add, del-->
    /* This potentially overwrites a different client's setting, but ... */
    XChangeDeviceProperty(display, dev, touchpad_off_prop, XA_INTEGER, 8,
                          PropModeReplace, &data, 1);
    XFlush(display);
#endif	// <--del
}

static void
signal_handler(int signum)
{
    toggle_touchpad(True);

    if (pid_file)
        unlink(pid_file);
    kill(getpid(), signum);
}

static void
install_signal_handler(void)
{
    static int signals[] = {
        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT,
        SIGBUS, SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2, SIGPIPE,
        SIGALRM, SIGTERM,
#ifdef SIGPWR
        SIGPWR
#endif
    };
    int i;
    struct sigaction act;
    sigset_t set;

    sigemptyset(&set);
    act.sa_handler = signal_handler;
    act.sa_mask = set;
#ifdef SA_ONESHOT
    act.sa_flags = SA_ONESHOT;
#else
    act.sa_flags = 0;
#endif

    for (i = 0; i < sizeof(signals) / sizeof(int); i++) {
        if (sigaction(signals[i], &act, NULL) == -1) {
            perror("sigaction");
            exit(2);
        }
    }
}

/**
 * Return non-zero if the keyboard state has changed since the last call.
 */
static int
keyboard_activity(Display * display)
{
    static unsigned char old_key_state[KEYMAP_SIZE];
    unsigned char key_state[KEYMAP_SIZE];
    int i;
    int ret = 0;

    XQueryKeymap(display, (char *) key_state);

    for (i = 0; i < KEYMAP_SIZE; i++) {
        if ((key_state[i] & ~old_key_state[i]) & keyboard_mask[i]) {
            ret = 1;
            break;
        }
    }
    if (ignore_modifier_combos) {
        for (i = 0; i < KEYMAP_SIZE; i++) {
            if (key_state[i] & ~keyboard_mask[i]) {
                ret = 0;
                break;
            }
        }
    }
    for (i = 0; i < KEYMAP_SIZE; i++)
        old_key_state[i] = key_state[i];
    return ret;
}

static double
get_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static void
main_loop(Display * display, double idle_time, int poll_delay)
{
    double last_activity = 0.0;
    double current_time;

    keyboard_activity(display);

    for (;;) {
        current_time = get_time();
        if (keyboard_activity(display))
            last_activity = current_time;

        /* If system times goes backwards, touchpad can get locked. Make
         * sure our last activity wasn't in the future and reset if it was. */
        if (last_activity > current_time)
            last_activity = current_time - idle_time - 1;

        if (current_time > last_activity + idle_time) { /* Enable touchpad */
            toggle_touchpad(True);
        }
        else {                  /* Disable touchpad */
            toggle_touchpad(False);
        }

        usleep(poll_delay);
    }
}

static void
clear_bit(unsigned char *ptr, int bit)
{
    int byte_num = bit / 8;
    int bit_num = bit % 8;

    ptr[byte_num] &= ~(1 << bit_num);
}

static void
setup_keyboard_mask(Display * display, int ignore_modifier_keys)
{
    XModifierKeymap *modifiers;
    int i;

    for (i = 0; i < KEYMAP_SIZE; i++)
        keyboard_mask[i] = 0xff;

    if (ignore_modifier_keys) {
        modifiers = XGetModifierMapping(display);
        for (i = 0; i < 8 * modifiers->max_keypermod; i++) {
            KeyCode kc = modifiers->modifiermap[i];

            if (kc != 0)
                clear_bit(keyboard_mask, kc);
        }
        XFreeModifiermap(modifiers);
    }
}

/* ---- the following code is for using the xrecord extension ----- */
#if 0	// del -->
#ifdef HAVE_X11_EXTENSIONS_RECORD_H

#define MAX_MODIFIERS 16

/* used for exchanging information with the callback function */
struct xrecord_callback_results {
    XModifierKeymap *modifiers;
    Bool key_event;
    Bool non_modifier_event;
    KeyCode pressed_modifiers[MAX_MODIFIERS];
};

/* test if the xrecord extension is found */
Bool
check_xrecord(Display * display)
{

    Bool found;
    Status status;
    int major_opcode, minor_opcode, first_error;
    int version[2];

    found = XQueryExtension(display,
                            "RECORD",
                            &major_opcode, &minor_opcode, &first_error);

    status = XRecordQueryVersion(display, version, version + 1);
    if (verbose && status) {
        printf("X RECORD extension version %d.%d\n", version[0], version[1]);
    }
    return found;
}

/* called by XRecordProcessReplies() */
void
xrecord_callback(XPointer closure, XRecordInterceptData * recorded_data)
{

    struct xrecord_callback_results *cbres;
    xEvent *xev;
    int nxev;

    cbres = (struct xrecord_callback_results *) closure;

    if (recorded_data->category != XRecordFromServer) {
        XRecordFreeData(recorded_data);
        return;
    }

    nxev = recorded_data->data_len / 8;
    xev = (xEvent *) recorded_data->data;
    while (nxev--) {

        if ((xev->u.u.type == KeyPress) || (xev->u.u.type == KeyRelease)) {
            int i;
            int is_modifier = 0;

            cbres->key_event = 1;       /* remember, a key was pressed or released. */

            /* test if it was a modifier */
            for (i = 0; i < 8 * cbres->modifiers->max_keypermod; i++) {
                KeyCode kc = cbres->modifiers->modifiermap[i];

                if (kc == xev->u.u.detail) {
                    is_modifier = 1;    /* yes, it is a modifier. */
                    break;
                }
            }

            if (is_modifier) {
                if (xev->u.u.type == KeyPress) {
                    for (i = 0; i < MAX_MODIFIERS; ++i)
                        if (!cbres->pressed_modifiers[i]) {
                            cbres->pressed_modifiers[i] = xev->u.u.detail;
                            break;
                        }
                }
                else {          /* KeyRelease */
                    for (i = 0; i < MAX_MODIFIERS; ++i)
                        if (cbres->pressed_modifiers[i] == xev->u.u.detail)
                            cbres->pressed_modifiers[i] = 0;
                }

            }
            else {
                /* remember, a non-modifier was pressed. */
                cbres->non_modifier_event = 1;
            }
        }

        xev++;
    }

    XRecordFreeData(recorded_data);     /* cleanup */
}

static int
is_modifier_pressed(const struct xrecord_callback_results *cbres)
{
    int i;

    for (i = 0; i < MAX_MODIFIERS; ++i)
        if (cbres->pressed_modifiers[i])
            return 1;

    return 0;
}

void
record_main_loop(Display * display, double idle_time)
{

    struct xrecord_callback_results cbres;
    XRecordContext context;
    XRecordClientSpec cspec = XRecordAllClients;
    Display *dpy_data;
    XRecordRange *range;
    int i;

    dpy_data = XOpenDisplay(NULL);      /* we need an additional data connection. */
    range = XRecordAllocRange();

    range->device_events.first = KeyPress;
    range->device_events.last = KeyRelease;

    context = XRecordCreateContext(dpy_data, 0, &cspec, 1, &range, 1);

    XRecordEnableContextAsync(dpy_data, context, xrecord_callback,
                              (XPointer) & cbres);

    cbres.modifiers = XGetModifierMapping(display);
    /* clear list of modifiers */
    for (i = 0; i < MAX_MODIFIERS; ++i)
        cbres.pressed_modifiers[i] = 0;

    while (1) {

        int fd = ConnectionNumber(dpy_data);
        fd_set read_fds;
        int ret;
        int disable_event = 0;
        struct timeval timeout;

        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        ret = select(fd + 1 /* =(max descriptor in read_fds) + 1 */ ,
                     &read_fds, NULL, NULL,
                     pad_disabled ? &timeout : NULL
                     /* timeout only required for enabling */ );

        if (FD_ISSET(fd, &read_fds)) {

            cbres.key_event = 0;
            cbres.non_modifier_event = 0;

            XRecordProcessReplies(dpy_data);

            /* If there are any events left over, they are in error. Drain them
             * from the connection queue so we don't get stuck. */
            while (XEventsQueued(dpy_data, QueuedAlready) > 0) {
                XEvent event;

                XNextEvent(dpy_data, &event);
                fprintf(stderr, "bad event received, major opcode %d\n",
                        event.type);
            }

            if (!ignore_modifier_keys && cbres.key_event) {
                disable_event = 1;
            }

            if (cbres.non_modifier_event &&
                !(ignore_modifier_combos && is_modifier_pressed(&cbres))) {
                disable_event = 1;
            }
        }

        if (disable_event) {
            /* adjust the enable_time */
            timeout.tv_sec = (int) idle_time;
            timeout.tv_usec = (idle_time - (double) timeout.tv_sec) * 1.e6;

            toggle_touchpad(False);
        }

        if (ret == 0 && pad_disabled) { /* timeout => enable event */
            toggle_touchpad(True);
        }

    }                           /* end while(1) */

    XFreeModifiermap(cbres.modifiers);
}
#endif                          /* HAVE_X11_EXTENSIONS_RECORD_H */

static int
dp_get_device(Display * dpy)
{
    XDevice *dev = NULL;
    XDeviceInfo *info = NULL;
    int ndevices = 0;
    Atom touchpad_type = 0;
    Atom *properties = NULL;
    int nprops = 0;
    int error = 0;

    touchpad_type = XInternAtom(dpy, XI_TOUCHPAD, True);
    touchpad_off_prop = XInternAtom(dpy, SYNAPTICS_PROP_OFF, True);
    info = XListInputDevices(dpy, &ndevices);

    while (ndevices--) {
        if (info[ndevices].type == touchpad_type) {
            dev = XOpenDevice(dpy, info[ndevices].id);
            if (!dev) {
                fprintf(stderr, "Failed to open device '%s'.\n",
                        info[ndevices].name);
                error = 1;
                goto unwind;
            }

            properties = XListDeviceProperties(dpy, dev, &nprops);
            if (!properties || !nprops) {
                fprintf(stderr, "No properties on device '%s'.\n",
                        info[ndevices].name);
                error = 1;
                goto unwind;
            }

            while (nprops--) {
                if (properties[nprops] == touchpad_off_prop)
                    break;
            }
            if (nprops < 0) {
                fprintf(stderr, "No synaptics properties on device '%s'.\n",
                        info[ndevices].name);
                error = 1;
                goto unwind;
            }

            break;              /* Yay, device is suitable */
        }
    }

 unwind:
    XFree(properties);
    XFreeDeviceList(info);
    if (!dev)
        fprintf(stderr, "Unable to find a synaptics device.\n");
    else if (error && dev) {
        XCloseDevice(dpy, dev);
        dev = NULL;
    }
    return dev;
}
#endif		//<--del

int
main(int argc, char *argv[])
{
    double idle_time = 1.0;
//  double idle_time = 2.0;
    int poll_delay = 200000;    /* 200 ms */
    int c;
    int use_xrecord = 0;

    /* Parse command line parameters */
    while ((c = getopt(argc, argv, "i:m:dtp:kKR?v")) != EOF) {
        switch (c) {
        case 'i':
            idle_time = atof(optarg);
            break;
        case 'm':
            poll_delay = atoi(optarg) * 1000;
            break;
        case 'd':
            background = 1;
            break;
//      case 't':
//          disable_state = TappingOff;
//          break;
        case 'p':
            pid_file = optarg;
            break;
        case 'k':
            ignore_modifier_keys = 1;
            break;
        case 'K':
            ignore_modifier_combos = 1;
            ignore_modifier_keys = 1;
            break;
//      case 'R':
//          use_xrecord = 1;
//          break;
        case 'v':
            verbose = 1;
            break;
        case '?':
        default:
            usage();
            break;
        }
    }
    if (idle_time <= 0.0)
        usage();

    /* Open a connection to the X server */
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Can't open display.\n");
        exit(2);
    }

#if 0	// del-->
   if (!(dev = dp_get_device(display)))
        exit(2);
#endif	// <--del

    /* Install a signal handler to restore synaptics parameters on exit */
    install_signal_handler();

    if (background) {
        pid_t pid;

        if ((pid = fork()) < 0) {
            perror("fork");
            exit(3);
        }
        else if (pid != 0)
            exit(0);

        /* Child (daemon) is running here */
        setsid();               /* Become session leader */
        chdir("/");             /* In case the file system gets unmounted */
        umask(0);               /* We don't want any surprises */
        if (pid_file) {
            FILE *fd = fopen(pid_file, "w");

            if (!fd) {
                perror("Can't create pid file");
                exit(3);
            }
            fprintf(fd, "%d\n", getpid());
            fclose(fd);
        }
    }

	touchpad_type();		//add

    pad_disabled = False;
    store_current_touchpad_state();

// add-->
    if( previous_state == TouchpadOff ) {
        fprintf(stderr, "TouchPad device is Not Enable.\n");
        exit(2);
    }
// <--add

#if 0	// del-->
#ifdef HAVE_X11_EXTENSIONS_RECORD_H
    if (use_xrecord) {
        if (check_xrecord(display))
            record_main_loop(display, idle_time);
        else {
            fprintf(stderr, "Use of XRecord requested, but failed to "
                    " initialize.\n");
            exit(4);
        }
    }
    else
#endif                          /* HAVE_X11_EXTENSIONS_RECORD_H */
#endif	//<--del
    {
        setup_keyboard_mask(display, ignore_modifier_keys);

        /* Run the main loop */
        main_loop(display, idle_time, poll_delay);
    }
    return 0;
}

/* vim: set noexpandtab tabstop=8 shiftwidth=4: */
