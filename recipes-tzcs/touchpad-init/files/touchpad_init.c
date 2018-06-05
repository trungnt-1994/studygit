#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "tvalzctl.h"

#define create_file_conf "\
if [ ! -f \"/etc/syntp_sensitive.conf\" ]; then\n\
echo \"4\" > /etc/syntp_sensitive.conf\n\
fi\n\
"
#define remove_file_conf "\
if [ -f \"/etc/syntp_sensitive.conf\" ]; then\n\
rm /etc/syntp_sensitive.conf\n\
fi\n\
"
#define synaptic_initialize "Synaptic_sst `cat /etc/syntp_sensitive.conf`"
#define	remove_synaptic_conf "rm /usr/share/X11/xorg.conf.d/50-syntp.conf"
#define remove_serioraw_module "rmmod serio_raw"
char alps_palm_rejection[50] = "sed -i \"10 a psmouse_tos setpalm ";

int main(int argc, char *argv[]) {
	int ret;
	int type_touchpad;

	ret = system("grep AlpsPS /var/log/SelectTouchPadName > /dev/null 2>&1");

	if(WIFEXITED(ret)){
		type_touchpad = WEXITSTATUS(ret);

		if( type_touchpad == 0 ) {
#ifdef DEBUG
			g_print ("Select Alps TouchPad (SelectTouchPadName) \n");
#endif
		}else{
#ifdef DEBUG
			g_print ("Select Synaptics TouchPad (SelectTouchPadName) \n");
#endif
		}
	}else{
		type_touchpad = 0xFF;
#ifdef DEBUG
		g_print ("Command : grep SelectTouchPadName - NG (Select Synaptics TouchPad) \n");
#endif
	}

	// ALPS Touch Pad ?
	if( type_touchpad == 0 ) {
		if (argv[1]!= NULL) {
			strcat(alps_palm_rejection,argv[1]);
			strcat(alps_palm_rejection,"& \" /root/.xinitrc");
		} else {
			strcat(alps_palm_rejection,"on");
			strcat(alps_palm_rejection,"& \" /root/.xinitrc");
		}
		system(remove_file_conf);
		system(remove_synaptic_conf);
		system(remove_serioraw_module);
		system(alps_palm_rejection);
		system("echo \"Initial ALPS Touch Pad\" > /var/log/TouchPad.log");
		system("echo \"Select ALPS Touch Pad \" >> /var/log/TouchPad.log");
	} else {
		system(create_file_conf);
		system(synaptic_initialize);
		system("echo \"Initial Synaptics Touch Pad\" > /var/log/TouchPad.log");
		system("echo \"Select Synaptics Touch Pad \" >> /var/log/TouchPad.log");
		system("cat /var/log/SelectTouchPadName >> /var/log/TouchPad.log");

		if( type_touchpad == 0xFF ) {
			system("echo \"Command grep SelectTouchPadName Errror \" >> /var/log/TouchPad.log");
		}
	}

	return;
}
