/************************************************
 * test_hci.c
 *
 * Copyright 2016-2017 TOSHIBA DEVELOPMENT & ENGINEERINGS CO.,LTD.
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2017/06/27  DME
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
#define DEBUG               1

/*--------------------------------------------------------------------
 * Resouces
 *------------------------------------------------------------------*/

/*--------------------------------------------------------------------
 * Global Data
 *------------------------------------------------------------------*/


/*-------------------------------------------------------------------
 * Function   : usage
 *------------------------------------------------------------------*/
static void
usage(void)
{
    fprintf(stderr,
            "Usage: test_hci -G or -S -C Subfunction Code  [-A Set data] \n");
    fprintf(stderr, "   -G Read Device Status.\n");
    fprintf(stderr, "   -S Control Device.\n");
    fprintf(stderr, "   -C Subfunction Code.\n");
    fprintf(stderr, "   -A Arg2 (detail specifications of arguments).\n");
    fprintf(stderr, "   -B Arg3 (detail specifications of arguments).\n");
    fprintf(stderr, "   -? Show this help message.\n");
    exit(1);
}

/*-------------------------------------------------------------------
 * Function   : main
 *------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
	int					c;
	int					fd;
	FILE 				*fd_set;
	int					file_flag = 0;
	char				*save_file;
	char				command_buf[128];
	ghci_interface		ghci_buf;
	unsigned int		in_Arg0 = 0;
	unsigned int		in_Arg1 = 0;
	unsigned int		in_Arg2 = 0;
	unsigned int		in_Arg3 = 0;
	unsigned int		in_Arg4 = 0;
	unsigned int		in_Arg5 = 0;

	memset(&command_buf[0], 0x00, sizeof(command_buf));

	/* Parse command line parameters */
	while ((c = getopt(argc, argv, "GSC:A:B:")) != EOF) {
		switch (c) {
			/* HCI Arg0 : Function Code */
			case 'G':
			case 'g':
			case 'S':
			case 's':
				if( c == 'G' || c == 'g' ) {
					in_Arg0 = 0xfe00;		// HCI Get Function
				}else{
					in_Arg0 = 0xff00;		// HCI Set Function
				}
			break;

			/* HCI Arg1 : Subfunction Code */
			case 'C':
			case 'c':
				in_Arg1 = (unsigned int)strtol(optarg, 0, 16);
				if( in_Arg1 >= 0x0001 && in_Arg1 <= 0x00CA ) {
					;
				}else{
					printf("Subfunction Code Error! (0x%04X)\n",in_Arg1);
					usage();
					exit(2);
				}
			break;

			/* HCI Arg2 : detail specifications of arguments */
			case 'A':
			case 'a':
				in_Arg2 = (unsigned int)strtol(optarg, 0, 16);
				if( in_Arg2 >= 0x0000 && in_Arg2 <= 0xFFFF ) {
					;
				}else{
					printf("Subfunction Code Error(Arg2)! (0x%04X)\n",in_Arg2);
					usage();
					exit(2);
				}
			break;

			/* HCI Arg3 : detail specifications of arguments */
			case 'B':
			case 'b':
				in_Arg3 = (unsigned int)strtol(optarg, 0, 16);
				if( in_Arg3 >= 0x0000 && in_Arg3 <= 0xFFFF ) {
					;
				}else{
					printf("Subfunction Code Error(Arg3)! (0x%04X)\n",in_Arg3);
					usage();
					exit(2);
				}
			break;

			case '?':
			default:
				usage();
				exit(1);
			break;
		}
	}

	fd = open("/dev/tvalz0", O_RDONLY);
	if (fd < 0){
		printf("Test HCI dev open  Error! \n");
		exit(2);
	}else{
		// HCI Get/Set Function
		ghci_buf.ghci_eax = in_Arg0;	// HCI Function Code
		ghci_buf.ghci_ebx = in_Arg1;	// HCI Subfunction Code
		ghci_buf.ghci_ecx = in_Arg2;	// HCI detail specifications of arguments
		ghci_buf.ghci_edx = in_Arg3;
		ghci_buf.ghci_esi = in_Arg4;
		ghci_buf.ghci_edi = in_Arg5;

		printf("Input : HCI Get/Set Function \n");
		printf("  Arg0 = 0x%04X \n", ghci_buf.ghci_eax);
		printf("  Arg1 = 0x%04X \n", ghci_buf.ghci_ebx);
		printf("  Arg2 = 0x%04X \n", ghci_buf.ghci_ecx);
		printf("  Arg3 = 0x%04X \n", ghci_buf.ghci_edx);
		printf("  Arg4 = 0x%04X \n", ghci_buf.ghci_esi);
		printf("  Arg5 = 0x%04X \n", ghci_buf.ghci_edi);

		ioctl(fd, IOCTL_TVALZ_GHCI,  &ghci_buf);
		close(fd);

		printf("Output : HCI Get/Set Function \n");
		printf("  Arg0 = 0x%04X \n", ghci_buf.ghci_eax);
		printf("  Arg1 = 0x%04X \n", ghci_buf.ghci_ebx);
		printf("  Arg2 = 0x%04X \n", ghci_buf.ghci_ecx);
		printf("  Arg3 = 0x%04X \n", ghci_buf.ghci_edx);
		printf("  Arg4 = 0x%04X \n", ghci_buf.ghci_esi);
		printf("  Arg5 = 0x%04X \n", ghci_buf.ghci_edi);
	}

    return 0;
}

