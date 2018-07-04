#define TZCS

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <linux/input.h>
#include <ctype.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <glib.h>
#include <math.h>
#include <signal.h>
#include <sys/mman.h>
#include <sched.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <libintl.h>
#include <locale.h>

#ifdef TZCS
#include "tvalzctl.h"
#endif

#define battery_alarm1 10
#define battery_alarm2 2
#define button_width 160
#define button_height 50
#define disp_alarm1 1
#define disp_alarm2 2
#define battery_scan 10000

const char *get_width_screen_resolution  = "xrandr | head -n1 | awk '{print $8}' | awk -F \",\" '{print $1}'";
const char *get_height_screen_resolution = "xrandr | head -n1 | awk '{print $10}' | awk -F \",\" '{print $1}'";

#ifdef TZCS
	const char *checkbatterycharge = "cat /proc/acpi/ac_adapter/ADP1/state | awk '{print $2}'"; //get battery status(Chagre or not)
	const char *getcurrentbattery = "cat /proc/acpi/battery/BAT1/state | awk '{if (NR==5) {print $3}}'";//get index of battery in the current
	const char *getfullbattery ="cat /proc/acpi/battery/BAT1/info | awk '{if(NR==3) {print $4}}'";//get index of battery when it is full

#else
	const char *checkbatterycharge = "cat /home/matsumoto/work/test/proc/acpi/ac_adapter/ADP1/state | awk '{print $2}'"; //get battery status(Chagre or not)
	const char *getcurrentbattery = "cat /home/matsumoto/work/test/proc/acpi/battery/BAT1/state | awk '{if (NR==5) {print $3}}'";//get index of battery in the current
	const char *getfullbattery ="cat /home/matsumoto/work/test/proc/acpi/battery/BAT1/info | awk '{if(NR==3) {print $4}}'";//get index of battery when it is full
#endif

const char *status_on = "on-line";

const char *alarm1_E1 = "<span size='24576'>Your battery is low. ";
const char *alarm1_E2 = "</span>\n<span size= '16384'>Please plug in your PC.                       </span>";

const char *alarm1_J1 = "<span size='24576'>バッテリーの残量が少なくなっています。 ";
const char *alarm1_J2 = "</span>\n<span size= '16384'>ACアダプターと電源コードを接続して充電してください。 　　　　　　　</span>";

/***** -add [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
const char *alarm1_D1 = "<span size='24576'>Der Akku ist fast leer. ";
const char *alarm1_D2 = "</span>\n<span size= '16384'>Bitte das Ladegerät anschließen.</span>";
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/

const char *alarm2_E1 = "<span size='24576'>Your battery is very low. ";
const char *alarm2_E2 = "</span>\n<span size= '16384'>Please plug in your PC immediately.                  </span>";

const char *alarm2_J1 = "<span size='24576'>バッテリーの残量が非常に少なくなっています。 ";
const char *alarm2_J2 = "</span>\n<span size= '16384'>今すぐACアダプターと電源コードを接続して充電してください。  　　　　　　　　</span>";

/***** -add [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
const char *alarm2_D1 = "<span size='24576'>Kritischer Akkustand. ";
const char *alarm2_D2 = "</span>\n<span size= '16384'>Bitte umgehend das Ladegerät anschließen.</span>";
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/

const char *alarm3_E = "(About %d%)";
const char *alarm3_J = "(約%d%)";
const char *alarm3_D = "(ca. %d%)";		/***** -add [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/

const char *button_E = "CLOSE";
const char *button_J = "閉じる";
const char *button_D = "schließen";		/***** -add [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/

const char *frame_color = "#000033";

const char *button_normal_color = "#000066";
const char *button_active_color = "#000099";
const char *button_prelight_color = "#0000CC";

const char *label_char_color = "#FFFFFF";


GtkWidget *window;
GtkWidget *window2;
GtkWidget *label1;
GtkWidget *label2;
GtkWidget *button;
GtkWidget *box;
GtkWidget *canvas;
GtkWidget *fixed;
GtkWidget *image;

GdkColor color;
GdkColor colorb1;
GdkColor colorb2;
GdkColor colorb3;
GdkColor colorchar;

GdkPixbuf *pixbuf;

PangoFontDescription *font_description;
int battery_timer_id;
unsigned int flags_battery1 = 0;
unsigned int flags_battery2 = 0;
unsigned int flags_close = 0;
char alarmtitle[256];
char battery[10];
unsigned int battery_now=0;
gint width=0;
gint height=0;
gint width_now=0;
gint height_now=0;

#ifdef TZCS

ghci_interface ghci_buf;

void ioctl_get_hci(int fd, int num, int Index) {
	int result;
	ghci_buf.ghci_eax = 0xfe00;
	ghci_buf.ghci_ebx = 0x00d0;
	ghci_buf.ghci_ecx = (ULONG)num;
	ghci_buf.ghci_edx = (ULONG)Index;
	ghci_buf.ghci_esi = 0x00;
	ghci_buf.ghci_edi = 0x00;
	
	result = ioctl(fd, IOCTL_TVALZ_GHCI,  &ghci_buf);	

}
int  get_language() {
  	unsigned int num;
  	int ret;
  	int fd;
	// tval open
	fd = open("/dev/tvalz0", O_RDONLY);
	if (fd < 0) {
		//error
		printf("Can't read file /dev/tvalz0\n");
	}

	// language
	num = VAL_LANGUAGE;
	ioctl_get_hci(fd, num, 0);
	if ((ghci_buf.ghci_eax & 0xFF00) != 0) {
		// error
		printf("Get language error \n");				
	}
	ret = ghci_buf.ghci_ecx & 0xFF; // 0: <English>（デフォルト）1: <Japanese> 2: <German>
	close(fd);
	return ret;
}

#endif

int get_info(const char *cmd) {
	int result=0;
	FILE *fp;
	char path[100];

	/* Open the command for reading. */
	fp = popen(cmd, "r");
	if (fp == NULL) {
		printf("Failed to run command #001\n");
		return 0;
	}
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof (path) - 1, fp) != NULL) {
				result = atoi(path);
	}
	/* close */
	pclose(fp);
	return result;
}


char *get_status(const char *cmd) {
	FILE *fp;
	char path[100];
	char *temp;
	/* Open the command for reading. */
	fp = popen(cmd, "r");
	if (fp == NULL) {
		printf("Failed to run command #002\n");
	}
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof (path) - 1, fp) != NULL) {
		temp = path;
	}
	/* close */
	pclose(fp);
	return strdup(temp);
}


char *trim(char *str) {
		char *end;
		while(isspace(*str)==true) str++;
		if(*str == 0)
				return str;
		end =str +strlen(str)-1;
		while(end > str && isspace(*end)) end--;
		*(end+1) = 0;
		return str;
}

/*
 * battery disp
 */
gboolean battery_disp(int disptype, unsigned int batteryindex) {

	width_now  = get_info(get_width_screen_resolution);
	height_now  = get_info(get_height_screen_resolution);
#ifndef TZCS
	printf("width_now %d \n ",width_now);
	printf("height_now %d \n ",height_now);
#endif
	if((width_now != width) || (height_now != height)){
		width = width_now;
		height = height_now;
		flags_battery1 = 0;
		flags_battery2 = 0;
		flags_close = 0;
		battery_now = 0;
		gtk_widget_destroy(window);
		gtk_widget_destroy(window2);
		gtk_main_quit();
		return(FALSE);
	}else{
		if(batteryindex != battery_now){
			battery_now = batteryindex;
#ifdef TZCS
/***** -mod [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
			switch(get_language()){
				case 1:		/* Japanese */
					snprintf(battery,12,alarm3_J,batteryindex);
					break;
				case 2:		/* German */
					snprintf(battery,12,alarm3_D,batteryindex);
					break;
				default:	/* English */
					snprintf(battery,12,alarm3_E,batteryindex);
					break;
			}
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
#else
			snprintf(battery,12,alarm3_J,batteryindex);

#endif
			if(disptype == disp_alarm1){

				flags_battery1 = 1;
#ifdef TZCS
/***** -mod [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
				switch(get_language()){
					case 1:		/* Japanese */
						strcpy(alarmtitle, alarm1_J1);
						strcat(alarmtitle, battery);
						strcat(alarmtitle, alarm1_J2);
						break;
					case 2:		/* German */
						strcpy(alarmtitle, alarm1_D1);
						strcat(alarmtitle, battery);
						strcat(alarmtitle, alarm1_D2);
						break;
					default:	/* English */
						strcpy(alarmtitle, alarm1_E1);
						strcat(alarmtitle, battery);
						strcat(alarmtitle, alarm1_E2);
						break;
				}
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
#else
				strcpy(alarmtitle, alarm1_J1);
				strcat(alarmtitle, battery);
				strcat(alarmtitle, alarm1_J2);
#endif
				gtk_label_set_markup(GTK_LABEL(label1),alarmtitle);
				gtk_widget_show_all (window);
///// Add 20161117 start /////
				gtk_widget_show_all (window2);
///				gtk_widget_hide(window2);
///// Add 20161117 end /////
			}else if(disptype == disp_alarm2){

				flags_battery2 = 1;
#ifdef TZCS
/***** -mod [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
				switch(get_language()){
					case 1:		/* Japanese */
						strcpy(alarmtitle, alarm2_J1);
						strcat(alarmtitle, battery);
						strcat(alarmtitle, alarm2_J2);
						break;
					case 2:		/* German */
						strcpy(alarmtitle, alarm2_D1);
						strcat(alarmtitle, battery);
						strcat(alarmtitle, alarm2_D2);
						break;
					default:	/* English */
						strcpy(alarmtitle, alarm2_E1);
						strcat(alarmtitle, battery);
						strcat(alarmtitle, alarm2_E2);
						break;
				}
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
#else
				strcpy(alarmtitle, alarm2_J1);
				strcat(alarmtitle, battery);
				strcat(alarmtitle, alarm2_J2);
#endif
				gtk_label_set_markup(GTK_LABEL(label1),alarmtitle);
				gtk_widget_show_all (window);
///// Add 20161117  start /////
				gtk_widget_show_all (window2);
///				gtk_widget_hide(window2);
///// Add 20161117 end /////
			}
		}
	}
	return(TRUE);
}

static gint cb_key_press (GtkWidget       *widget ,
                          GdkEventKey     *event ,
                          gpointer        data) {

#ifndef TZCS
     printf("key_press:keyval=%02X", event->keyval);
#endif

    return TRUE;

}

/*
 * Timer to battery check
 */
gboolean time_battery_check(gpointer nulldata) {

	char *battery_status_temp;
	int battery_charge;
	unsigned int batteryindex;

#ifndef TZCS
	g_print("battery alarm check start \n");
#endif
	/*get battery charge*/
	battery_status_temp=get_status(checkbatterycharge);
	/*get battery index*/
	batteryindex=(get_info(getcurrentbattery)*100)/get_info(getfullbattery);
	battery_charge = strcmp(trim(battery_status_temp),status_on);

#ifndef TZCS
	g_print("battery %d \n", batteryindex);
	g_print("batterycharge %d \n", battery_charge);
#endif
	if(batteryindex > battery_alarm2 && batteryindex <= battery_alarm1) {

		if(flags_battery2 == 1){
			flags_battery2 = 0;
			flags_close = 0;
		}
		if((battery_charge != 0) && (flags_close !=  1)) {
			battery_disp(disp_alarm1, batteryindex);
		}else if((flags_battery1 ==  1) && (battery_charge == 0)) {
			gtk_widget_hide(window);
			gtk_widget_hide(window2);
			flags_battery1 = 0;
			flags_close = 0;
			battery_now = 0;
		}

	}else if(batteryindex <=  battery_alarm2) {

		if(flags_battery1 == 1){
			flags_battery1 = 0;
			flags_close = 0;
		}

		if((battery_charge != 0) && (flags_close !=  1)){
			battery_disp(disp_alarm2, batteryindex);
		}else if((flags_battery2 ==  1) && (battery_charge == 0)) {
			gtk_widget_hide(window);
			gtk_widget_hide(window2);
			flags_battery2 = 0;
			flags_close = 0;
			battery_now = 0;
		}

	}else {
		if((flags_battery1 ==  1) || (flags_battery2 ==  1)) {
			flags_battery1 = 0;
			flags_battery2 = 0;
			flags_close = 0;
			battery_now = 0;
			gtk_widget_hide(window);
			gtk_widget_hide(window2);
		}
	}

#ifndef TZCS
	g_print("battery alarm check end \n");
#endif
	return TRUE;

}

/*
 * destroy battery alarm
 */

void destroy_battery_alarm( GtkWidget *widget, gpointer data ) {

	gtk_widget_hide(window);
	gtk_widget_hide(window2);
}

void destroy_battery_alarm_close( GtkWidget *widget, gpointer data ) {

	gtk_widget_hide(window);
	gtk_widget_hide(window2);
	flags_close = 1;
}

int main (int argc , char *argv[]){

    width  = get_info(get_width_screen_resolution);
    height = get_info(get_height_screen_resolution);
#ifndef TZCS
    printf("width %d ",width);
    printf("height %d\n",height);
#endif
    gdk_color_parse(frame_color,&color);

    gdk_color_parse(button_normal_color,&colorb1);
    gdk_color_parse(button_prelight_color,&colorb2);
    gdk_color_parse(button_active_color,&colorb3);

    gdk_color_parse(label_char_color,&colorchar);

	while(1){
#ifndef TZCS
		g_print("battery alarm start \n");
#endif
		gtk_init (&argc , &argv);
		
		// キーのみ受け取るウィンドウを作成
		window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW(window2) , "TZCS");
		gtk_widget_modify_bg(window2, GTK_STATE_NORMAL, &color);
		g_signal_connect(G_OBJECT(window2), "key-press-event", G_CALLBACK (cb_key_press), NULL);
		gtk_widget_add_events (window2, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
///// Del 20161117 start /////
//		gtk_widget_show_all (window2);
//		gtk_widget_hide(window2);
///// Del 20161117 end /////

		window = gtk_window_new (GTK_WINDOW_POPUP);
		gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
		gtk_window_set_title (GTK_WINDOW (window),"TZCS");
		gtk_widget_set_size_request (window,width,height);
		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
		gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);

		box = gtk_vbox_new(GTK_ORIENTATION_VERTICAL, 0);
		gtk_container_add(GTK_CONTAINER(window), box);

#ifdef TZCS
		// ウィンドウは通常タイトルバーなどの装飾なしを設定
		gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
#endif
		// ウィンドウのトップをキープする様に設定
		gtk_window_set_keep_above (GTK_WINDOW(window), TRUE);
		// 現在のウィンドウにフォーカスをあわせる
		gtk_widget_grab_focus (window);
		gtk_widget_set_can_focus(window, TRUE);
		// タスクバーでウインドウを表示しないよう頼んでいるヒントをセット
		gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), TRUE);
		// pagerでウインドウを表示しないよう頼んでいるヒントをセット
		gtk_window_set_skip_pager_hint (GTK_WINDOW (window), TRUE);
		// このウインドウをmap上で入力フォーカスを受けさせる様にセット
		gtk_window_set_focus_on_map(GTK_WINDOW(window),TRUE);
		gtk_window_stick(GTK_WINDOW(window));

		label1 = gtk_label_new("");
		gtk_misc_set_alignment(GTK_MISC(label1), 0.5, 0.9);
		gtk_label_set_justify(GTK_LABEL(label1),GTK_JUSTIFY_CENTER);
		font_description = pango_font_description_from_string( "font-jis-misc Boald");

		gtk_widget_modify_font(label1,font_description);
		gtk_widget_modify_fg(label1, GTK_STATE_NORMAL, &colorchar);
		gtk_box_pack_start(GTK_BOX(box), label1, TRUE, TRUE, 0);

		g_signal_connect(GTK_OBJECT (window), "delete-event",G_CALLBACK(destroy_battery_alarm),NULL );
		g_signal_connect(GTK_OBJECT (window), "destroy", G_CALLBACK (destroy_battery_alarm), NULL);

		fixed = gtk_fixed_new();
		gtk_box_pack_start(GTK_BOX(box), fixed, TRUE, TRUE, 0);

		button = gtk_button_new();

		gtk_widget_modify_bg(button, GTK_STATE_NORMAL, &colorb1);
		gtk_widget_modify_bg(button, GTK_STATE_ACTIVE, &colorb2);
		gtk_widget_modify_bg(button, GTK_STATE_PRELIGHT, &colorb3);

#ifdef TZCS
/***** -mod [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
		switch(get_language()){
			case 1:		/* Japanese */
				label2 = gtk_label_new(button_J);
				break;
			case 2:		/* German */
				label2 = gtk_label_new(button_D);
				break;
			default:	/* English */
				label2 = gtk_label_new(button_E);
				break;
		}
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
#else
		label2 = gtk_label_new(button_J);
#endif
		font_description = pango_font_description_from_string( "font-jis-misc Boald 14");
		gtk_widget_modify_font(label2,font_description);
		gtk_widget_modify_fg(label2, GTK_STATE_NORMAL, &colorchar);
		gtk_widget_modify_fg(label2, GTK_STATE_ACTIVE, &colorchar);
		gtk_widget_modify_fg(label2, GTK_STATE_PRELIGHT, &colorchar);

		gtk_container_add(GTK_CONTAINER (button), label2);

		gtk_fixed_put(GTK_FIXED(fixed),button,(width*0.6),0);
		gtk_widget_set_size_request(button, button_width, button_height);
		g_signal_connect(button, "clicked", G_CALLBACK(destroy_battery_alarm_close), NULL);

		time_battery_check(NULL);

		battery_timer_id = g_timeout_add(battery_scan,(GSourceFunc) time_battery_check,(gpointer) NULL);

		gtk_main ();
	}
	return 0;
}
