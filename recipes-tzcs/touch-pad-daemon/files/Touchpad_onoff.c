/************************************************
 * Touchpad_onoff.c
 *
 * Copyright 2016-2017 TOSHIBA DEVELOPMENT & ENGINEERINGS CO.,LTD.
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2017/08/22  DME
 *  - New
 *  -  
 *
 ************************************************/
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "tvalzctl.h"

/*--------------------------------------------------------------------
 * Global Definitions
 *------------------------------------------------------------------*/
//#define DEBUG               1

/* TouchPad Settings Structure Version */
#define	MAJOR_VERSION		0x01
#define	MINOR_VERSION		0x02

/*--------------------------------------------------------------------
 * Resouces
 *------------------------------------------------------------------*/
typedef struct {
	gchar		gMajor_version;
	gchar		gMinor_version;
	gint		gTouchPad_onoff;
	double		gWait_Time;
	double		gPoll_Delay;
	gboolean	gPalmRejection_Active;
	gboolean	Malfunction_function;
} TouchPad_Settings;

char *touchpad_file = "/var/tmp/TouchPad_Settings.inf";

/*--------------------------------------------------------------------
 * Global Data
 *------------------------------------------------------------------*/
TouchPad_Settings	gTouchPad_Settings;

/*-------------------------------------------------------------------
 * Function   : usage
 *------------------------------------------------------------------*/
static void
usage(void)
{
    fprintf(stderr, "Usage: Touchpad_onoff -E or -D or -P or -S \n");
    fprintf(stderr, "   -E TouchPad Enable Set to TouchPad_Settings.inf & Save to BIOS.\n");
    fprintf(stderr, "   -D TouchPad Disable Set to TouchPad_Settings.inf & Save to BIOS.\n");
    fprintf(stderr, "   -P Print TouchPad_Settings.inf \n");
    fprintf(stderr, "   -S Set TouchPad Enable/Disable from TouchPad_Settings.inf value.\n");
//  fprintf(stderr, "   -S Set HCI TouchPad Enable/Disable from TouchPad_Settings.inf value.\n");
    fprintf(stderr, "   -? Show this help message.\n");
    exit(1);
}

/*-------------------------------------------------------------------
 * Function   : main
 *------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
	int				c;
	int				fd_touchpad;
	ssize_t 		size;
	off_t			offset;
	gint			TouchPad_onoff = 0xF;
	int				Print_flag = 0;
	int				HCI_flag = 0;
	struct stat		st;
	ghci_interface	ghci_buf_touchpad;

	/* Parse command line parameters */
	while ((c = getopt(argc, argv, "EDPS")) != EOF) {
		switch (c) {
			case 'E':
				TouchPad_onoff = 1;
			break;

			case 'D':
				TouchPad_onoff = 0;
			break;

			case 'P':
				Print_flag = 1;
			break;

			case 'S':
				HCI_flag = 1;
			break;

			case '?':
			default:
				usage();
				exit(1);
			break;
		}
	}

	if(stat(touchpad_file, &st) != 0){
#ifdef DEBUG
		g_print ("initial_setting : Since there is no settings file, it reads from BIOS.\n");
#endif
		/* TouchPad settings read from BIOS */
		system("tenco fvr 3 TouchPad_Settings.inf > /dev/null 2>&1");

		/* Copy to a save folder from current folder. */
		system("cp TouchPad_Settings.inf /var/tmp/TouchPad_Settings.inf > /dev/null 2>&1");
	}

	fd_touchpad = open(touchpad_file, O_SYNC | O_RDWR);

	if (fd_touchpad < 0) {
#ifdef DEBUG
		g_print ("Touchpad_onoff : File Open Error (settings file)\n");
#endif
		exit(2);
	}else{

		size = read(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));
#ifdef DEBUG
		g_print ("Touchpad_onoff : Read %s %d/%d\n",touchpad_file, size, sizeof(gTouchPad_Settings));
#endif

		if( Print_flag == 1 ) {
			g_print("Structure Version : %02X.%02X \n", (int)gTouchPad_Settings.gMajor_version, (int)gTouchPad_Settings.gMinor_version);
			g_print(" TouchPad ON/OFF : %d \n", (int)gTouchPad_Settings.gTouchPad_onoff);
			g_print(" PalmRejection : %d \n", (int)gTouchPad_Settings.gPalmRejection_Active);
			g_print(" Malfunction : %d \n", (int)gTouchPad_Settings.Malfunction_function);
			g_print(" Wait Time : %f \n", gTouchPad_Settings.gWait_Time);
			g_print(" Poll Delay : %f \n", gTouchPad_Settings.gPoll_Delay);
			close(fd_touchpad);
			exit(0);
		}

		if( gTouchPad_Settings.gMajor_version != MAJOR_VERSION
		||  gTouchPad_Settings.gMinor_version != MINOR_VERSION ) {
#ifdef DEBUG
			g_print ("Touchpad_onoff : TouchPad Settings Structure Version Discord (Set with default)\n");
#endif
			/* TouchPad Settings Structure Version Discord (Set with default) */
			memset(&gTouchPad_Settings, 0x00, sizeof(gTouchPad_Settings) );
			gTouchPad_Settings.gMajor_version = MAJOR_VERSION;
			gTouchPad_Settings.gMinor_version = MINOR_VERSION;
			if( TouchPad_onoff == 0xF ) {
				gTouchPad_Settings.gTouchPad_onoff = 1;
			}else{
				gTouchPad_Settings.gTouchPad_onoff = TouchPad_onoff;
			}
			gTouchPad_Settings.gPalmRejection_Active = 1;
			gTouchPad_Settings.Malfunction_function = 1;
			gTouchPad_Settings.gWait_Time = 1000;
			gTouchPad_Settings.gPoll_Delay = 200;
		}else{
#ifdef DEBUG
			g_print ("Touchpad_onoff : TouchPad Settings Structure Version is OK (%d)\n",TouchPad_onoff);
#endif
			if( TouchPad_onoff == 0 || TouchPad_onoff == 1 )
				gTouchPad_Settings.gTouchPad_onoff = TouchPad_onoff;
		}

		offset = lseek(fd_touchpad, 0, SEEK_SET);
		size = write(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));
#ifdef DEBUG
		g_print ("Touchpad_onoff : Write %s %d/%d\n",touchpad_file, size, sizeof(gTouchPad_Settings));
#endif
		close(fd_touchpad);
	}

	/* Copy to a current folder from save folder. */
	system("cp /var/tmp/TouchPad_Settings.inf TouchPad_Settings.inf  > /dev/null 2>&1");

	/* TouchPad settings write to BIOS */
	system("tenco fvw 3 TouchPad_Settings.inf  > /dev/null 2>&1");
#ifdef DEBUG
	g_print ("Touchpad_onoff : Write %s to BIOS \n",touchpad_file);
#endif

	if( HCI_flag == 1 ) {
		/* TouchPad Enable/Disable Set to HCI */
		if( gTouchPad_Settings.gTouchPad_onoff == 1 ) {
			system("Touchpad_ctrl enable > /dev/null 2>&1");		// ON (Enable)
		}else{
			system("Touchpad_ctrl disable > /dev/null 2>&1");		// OFF (Disable)
		}
	}

    return 0;
}

