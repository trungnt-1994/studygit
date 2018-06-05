/************************************************
 * syndaemon_start.c
 *
 * Copyright 2016-2017 TOSHIBA DEVELOPMENT & ENGINEERINGS CO.,LTD.
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2017/08/04  DME
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

/*-------------------------------------------------------------------
 * Function   : main
 *------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
	int					fd_touchpad;
	ssize_t				size;
	char				command_buf[128];
	int					temp;
	struct stat			st;
	TouchPad_Settings	gTouchPad_Settings;
	int					ret;
	int					type_alps;
	ghci_interface		ghci_buf_touchpad;

	/* Use TouchPad settings file? */
	if(stat(touchpad_file, &st) != 0){
#ifdef DEBUG
		g_print ("syndaemon_start : Since there is no settings file, it reads from BIOS.\n");
#endif
		/* TouchPad settings read from BIOS */
		system("tenco fvr 3 TouchPad_Settings.inf > /dev/null 2>&1");

		/* Copy to a save folder from current folder. */
		system("cp TouchPad_Settings.inf /var/tmp/TouchPad_Settings.inf > /dev/null 2>&1");
	}

	fd_touchpad = open(touchpad_file, O_SYNC | O_RDWR);

	if (fd_touchpad < 0) {
#ifdef DEBUG
		g_print ("syndaemon_start : Read File Open Error (settings file)\n");
#endif

		gTouchPad_Settings.gMajor_version = MAJOR_VERSION;
		gTouchPad_Settings.gMinor_version = MINOR_VERSION;
		gTouchPad_Settings.gTouchPad_onoff = 1;
		gTouchPad_Settings.gPalmRejection_Active = 1;
		gTouchPad_Settings.Malfunction_function = 1;
		gTouchPad_Settings.gWait_Time = 1000;
		gTouchPad_Settings.gPoll_Delay = 200;

		fd_touchpad = open(touchpad_file, O_SYNC | O_CREAT | O_RDWR, 0x666);

		if (fd_touchpad < 0) {
#ifdef DEBUG
			g_print ("syndaemon_start : Write File Open Error (settings file)\n");
#endif
		}else{
			lseek(fd_touchpad, 0, SEEK_SET);
			size = write(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));
			close(fd_touchpad);
		}

	}else{
		size = read(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));

		if( gTouchPad_Settings.gMajor_version != MAJOR_VERSION
		||  gTouchPad_Settings.gMinor_version != MINOR_VERSION ) {
#ifdef DEBUG
		g_print ("syndaemon_start : TouchPad Settings Structure Version Discord (Set with default)\n");
#endif
			/* TouchPad Settings Structure Version Discord (Set with default) */
			gTouchPad_Settings.gMajor_version = MAJOR_VERSION;
			gTouchPad_Settings.gMinor_version = MINOR_VERSION;
			gTouchPad_Settings.gTouchPad_onoff = 1;
			gTouchPad_Settings.gPalmRejection_Active = 1;
			gTouchPad_Settings.Malfunction_function = 1;
			gTouchPad_Settings.gWait_Time = 1000;
			gTouchPad_Settings.gPoll_Delay = 200;

			lseek(fd_touchpad, 0, SEEK_SET);
			write(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));
		}
		close(fd_touchpad);
	}

	/* syndaemon_tos kill */
	system("killall syndaemon_tos > /dev/null 2>&1");

	/* TouchPad Enable/Disable Set */
	if( gTouchPad_Settings.gTouchPad_onoff == 1 )
		system("Touchpad_ctrl enable > /dev/null 2>&1");		// ON (Enable)
	else
		system("Touchpad_ctrl disable > /dev/null 2>&1");		// OFF (Disable)

	/* TouchPad Disable Check */
	if(gTouchPad_Settings.gTouchPad_onoff == 0){
#ifdef DEBUG
		g_print ("TouchPad was disable.\n");
#endif
		exit(2);
	}

	/* Malfunction function Enable Check */
	if( gTouchPad_Settings.Malfunction_function == 1 )
	{
		memset( command_buf, 0x00, sizeof(command_buf) );

		temp = (int)(((int)gTouchPad_Settings.gWait_Time % 1000) / 100);

		/* Start syndaemon_tos */
		sprintf( command_buf, "syndaemon_tos -d -i %d.%d -m %d  > /dev/null 2>&1",(int)gTouchPad_Settings.gWait_Time/1000, (int)temp, (int)gTouchPad_Settings.gPoll_Delay );
#ifdef DEBUG
		g_print ("Command : %s\n", command_buf);
#endif
		system( command_buf );
	}

	/* TouchPad Type Check & PalmRejection Enable Check */
	ret = system("grep AlpsPS /var/log/SelectTouchPadName > /dev/null 2>&1");
	if(WIFEXITED(ret)){
		type_alps = WEXITSTATUS(ret);

		if( gTouchPad_Settings.gPalmRejection_Active == 1 )
		{
			/* PalmRejection Enable */
			if( type_alps == 0 ) {
				system("psmouse_tos setpalm on > /dev/null 2>&1");
			}else{
				/* TODO : Synaptics TouchPad Setings */
				;
			}
		}else{
			/* PalmRejection Disable */
			if( type_alps == 0 ) {
				system("psmouse_tos setpalm off > /dev/null 2>&1");
			}else{
				/* TODO : Synaptics TouchPad Setings */
				;
			}
		}
	}else{
		type_alps = -1;
#ifdef DEBUG
		g_print ("Command : grep SelectTouchPadName - NG\n");
#endif
	}

    return 0;
}

