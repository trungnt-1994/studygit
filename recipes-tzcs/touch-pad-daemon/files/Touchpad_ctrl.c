/************************************************
 * Touchpad_ctrl.c
 *
 * Copyright 2016-2017 TOSHIBA DEVELOPMENT & ENGINEERINGS CO.,LTD.
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2017/11/13  DME
 *  - New
 *  -  
 *
 ************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "tvalzctl.h"

int main(int argc, char *argv[]) {
	int ret;
	int type_touchpad;

    if (argc == 2)
    {
		ret = system("grep AlpsPS /var/log/SelectTouchPadName > /dev/null 2>&1");

		if(WIFEXITED(ret)){
			type_touchpad = WEXITSTATUS(ret);

			if( type_touchpad == 0 ) {
				g_print ("Select Alps TouchPad (SelectTouchPadName) \n");
				type_touchpad = 0;
			}else{
				g_print ("Select Synaptics TouchPad (SelectTouchPadName) \n");
				type_touchpad = 1;
			}
		}else{
			type_touchpad = 0xFF;
			g_print ("Command : grep SelectTouchPadName - NG (Select Synaptics TouchPad) \n");
		}

		/* ALPS Touch Pad ?		*/
		if( type_touchpad == 0 ) {
			/************************************/
			/* ALPS Touch Pad					*/
			/************************************/
			int				fd_touchpad;
			ghci_interface	ghci_buf_touchpad;

			fd_touchpad = open("/dev/tvalz0", O_RDONLY);

			if (fd_touchpad < 0){
			}else{
				// Get HCI Touch Pad Status
				ghci_buf_touchpad.ghci_eax = 0xfe00;			// HCI Set Function
				ghci_buf_touchpad.ghci_ebx = 0x005f;			// Device Output Status
				ghci_buf_touchpad.ghci_ecx = 0x00;				// Disable/Enable
				ghci_buf_touchpad.ghci_edx = 0x00;				// Touch Pad
				ghci_buf_touchpad.ghci_esi = 0x00;
				ghci_buf_touchpad.ghci_edi = 0x00;
				ioctl(fd_touchpad, IOCTL_TVALZ_GHCI, &ghci_buf_touchpad);

				if( ghci_buf_touchpad.ghci_ecx == 0x00 ) {			// Touch Pad Disable?
					// Set HCI Touch Pad Status Enable
					ghci_buf_touchpad.ghci_eax = 0xff00;			// HCI Set Function
					ghci_buf_touchpad.ghci_ebx = 0x005f;			// Device Output Status
					ghci_buf_touchpad.ghci_ecx = 0x01;				// Enable
					ghci_buf_touchpad.ghci_edx = 0x00;				// Touch Pad
					ghci_buf_touchpad.ghci_esi = 0x00;
					ghci_buf_touchpad.ghci_edi = 0x00;
					ioctl(fd_touchpad, IOCTL_TVALZ_GHCI, &ghci_buf_touchpad);
				}
				close(fd_touchpad);
			}

			if (strcmp(argv[1], "enable") == 0) {
				/* Set TouchPad and Pointing stick Enable by psmouse_tos Command	*/
				system("psmouse_tos disdev > /dev/null 2>&1");
			}else if (strcmp(argv[1], "disable") == 0) {
				/* Set TouchPad Only Disable by psmouse_tos Command					*/
				system("psmouse_tos disdev t > /dev/null 2>&1");
			}
		} else {
			/************************************/
			/* Synaptics & Other Touch Pad		*/
			/************************************/
			ULONG			TouchPad_onoff = 0xFF;
			int				fd_touchpad;
			ghci_interface	ghci_buf_touchpad;

			if (strcmp(argv[1], "enable") == 0) {
				/* TouchPad Enable by HCI		*/
				TouchPad_onoff = 1;
			}else if (strcmp(argv[1], "disable") == 0) {
				/* TouchPad Disable by HCI		*/
				TouchPad_onoff = 0;
			}

			if(TouchPad_onoff != 0xFF) {
				fd_touchpad = open("/dev/tvalz0", O_RDONLY);

				if (fd_touchpad < 0){
				}else{
					ghci_buf_touchpad.ghci_eax = 0xff00;			// HCI Set Function
					ghci_buf_touchpad.ghci_ebx = 0x005f;			// Device Output Status
					ghci_buf_touchpad.ghci_ecx = TouchPad_onoff;	// Disable/Enable
					ghci_buf_touchpad.ghci_edx = 0x00;				// Touch Pad
					ghci_buf_touchpad.ghci_esi = 0x00;
					ghci_buf_touchpad.ghci_edi = 0x00;
					ioctl(fd_touchpad, IOCTL_TVALZ_GHCI, &ghci_buf_touchpad);
					close(fd_touchpad);
				}
			}
		}
	}else{
	}

	return;
}
