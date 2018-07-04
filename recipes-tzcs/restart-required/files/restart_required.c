/**
****************************************************************
* Copyright(C) 2017 TOSHIBA CORPORATION. All rights reserved. *
****************************************************************
**/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <time.h>

#define CONTENT 		"[Security update: Restart required]\n\nThis PC will restart automatically after 60 seconds.\n"
#define MANUAL 			"[r]	key : Restart now\n[p]	key : Remind me in 10 minutes\n"
#define RESTART 		"Restart Now"
#define POSTPONE 		"Postpone"
#define BG_COLOR		"#000099"
#define BTN_COLOR		"#0072C4"
#define BTN_BORDER_COLOR 	"#FFFFFF"
#define RESTART_TIMEOUT 	60
#define REMIND_TIMEOUT 		600
//#define DEBUG

static const char *get_width_screen_resolution	= "xrandr | grep ' connected' | grep '+0+0' | awk '{print $3}' | cut -d'x' -f1";
static const char *get_height_screen_resolution	= "xrandr | grep ' connected' | grep '+0+0' | awk '{print $3}' | cut -d'x' -f2 | cut -d'+' -f1";
static const char *format_content 	= "<span font_desc=\"20.0\" color=\"white\">\%s</span>";
static const char *format_manual	= "<span font_desc=\"15.0\" color=\"white\">\%s</span>";
static const char *format_button	= "<span font_desc=\"12.0\" color=\"white\">\%s</span>";
static const char *content_msg1		= "[Security update: Restart required]\n\nThis PC will restart automatically after %d seconds.\n";
static const char *content_msg2		= "[Security update: Restart required]\n\nThis PC will restart automatically after %d second.\n";

int get_info(const char *cmd);

void restart();
void postpone(GtkWidget *widget, gpointer data);
void set_window_size(GtkWidget *widget);
void btn_restart_clicked(GtkWidget *widget, gpointer data);
void btn_postpone_clicked(GtkWidget *widget, gpointer data);

gboolean key_press_handle(GtkWidget *widget, GdkEventKey *kevent, gpointer data);
gboolean gb_postpone(GtkWidget *widget);
gboolean gb_restart_automatic(GtkWidget *widget);

GtkWidget *window;
GtkWidget *lb_content;

int timer_postpone_id;
int timer_restart_id;
int restart_timeout = RESTART_TIMEOUT;
int screen_width;
int screen_height;

char *markuptitle;

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *content_box;
	GtkWidget *manual_box;
	GtkWidget *valign_content;
	GtkWidget *halign_manual;
	GtkWidget *halign_button;
	GtkWidget *lb_manual;
	GtkWidget *lb_restart;
	GtkWidget *lb_postpone;
	GtkWidget *btn_restart;
	GtkWidget *btn_postpone;
	GtkWidget *restart_eventbox;
	GtkWidget *postpone_eventbox;

	GdkColor bg_color;
	GdkColor btn_color;
	GdkColor btn_border_color;

	guint content_padding_top;
	guint button_padding_bottom;
	guint button_padding_right;

	screen_width = get_info(get_width_screen_resolution);
	screen_height = get_info(get_height_screen_resolution);
	content_padding_top = (int) (screen_height / 3) - 10;
	button_padding_bottom = (int) (screen_height / 3);
	button_padding_right = (int) (screen_width / 4);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	vbox 			= gtk_vbox_new(3, FALSE);
	hbox 			= gtk_hbox_new(2, FALSE);
	content_box		= gtk_vbox_new(1, FALSE);
	manual_box 		= gtk_vbox_new(1, FALSE);

	valign_content 		= gtk_alignment_new(0.5, 0.5, 0, 0);
	halign_manual 		= gtk_alignment_new(0.35, 0, 0, 0);
	halign_button 		= gtk_alignment_new(1, 0, 0, 0);

	lb_content 		= gtk_label_new(CONTENT);
	lb_manual 		= gtk_label_new(MANUAL);
	lb_restart 		= gtk_label_new(RESTART);
	lb_postpone 		= gtk_label_new(POSTPONE);

	btn_restart 		= gtk_button_new();
	btn_postpone 		= gtk_button_new();

	restart_eventbox 	= gtk_event_box_new();
	postpone_eventbox 	= gtk_event_box_new();

	/* Set color */
	gdk_color_parse(BG_COLOR, &bg_color);
	gdk_color_parse(BTN_COLOR, &btn_color);
	gdk_color_parse(BTN_BORDER_COLOR, &btn_border_color);

	/* Create window */
	set_window_size(window);
	gtk_widget_modify_bg(GTK_WIDGET(window), GTK_STATE_NORMAL, &bg_color);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);
	gtk_window_set_focus_on_map(GTK_WINDOW(window), TRUE);
	gtk_widget_grab_focus(GTK_WIDGET(window));

	gtk_alignment_set_padding(GTK_ALIGNMENT(valign_content), content_padding_top, 0, 0, 0);
	gtk_alignment_set_padding(GTK_ALIGNMENT(halign_button), 0, button_padding_bottom, 0, button_padding_right);

	/* Create labels */
	markuptitle = g_markup_printf_escaped(format_content, CONTENT);
	gtk_label_set_justify(GTK_LABEL(lb_content), GTK_JUSTIFY_CENTER);
	gtk_label_set_markup(GTK_LABEL(lb_content), markuptitle);
	g_free(markuptitle);

	markuptitle = g_markup_printf_escaped(format_manual, MANUAL);
	gtk_label_set_markup(GTK_LABEL(lb_manual), markuptitle);
	g_free(markuptitle);

	markuptitle = g_markup_printf_escaped(format_button, RESTART);
	gtk_label_set_markup(GTK_LABEL(lb_restart), markuptitle);
	g_free(markuptitle);

	markuptitle = g_markup_printf_escaped(format_button, POSTPONE);
	gtk_label_set_markup(GTK_LABEL(lb_postpone), markuptitle);
	g_free(markuptitle);

	/* Create Restart Now button */
	gtk_widget_set_size_request(btn_restart, 150, 32);
	gtk_widget_modify_bg(GTK_WIDGET(btn_restart), GTK_STATE_NORMAL, &btn_color);
	gtk_widget_modify_bg(GTK_WIDGET(btn_restart), GTK_STATE_PRELIGHT, &btn_border_color);
	gtk_widget_modify_bg(GTK_WIDGET(restart_eventbox), GTK_STATE_NORMAL, &btn_color);
	gtk_widget_modify_bg(GTK_WIDGET(restart_eventbox), GTK_STATE_PRELIGHT, &btn_color);
	gtk_widget_modify_bg(GTK_WIDGET(restart_eventbox), GTK_STATE_ACTIVE, &btn_color);

	/* Create Postpone button */
	gtk_widget_set_size_request(btn_postpone, 150, 32);
	gtk_widget_modify_bg(GTK_WIDGET(btn_postpone), GTK_STATE_NORMAL, &btn_color);
	gtk_widget_modify_bg(GTK_WIDGET(btn_postpone), GTK_STATE_PRELIGHT, &btn_border_color);
	gtk_widget_modify_bg(GTK_WIDGET(postpone_eventbox), GTK_STATE_NORMAL, &btn_color);
	gtk_widget_modify_bg(GTK_WIDGET(postpone_eventbox), GTK_STATE_PRELIGHT, &btn_color);
	gtk_widget_modify_bg(GTK_WIDGET(postpone_eventbox), GTK_STATE_ACTIVE, &btn_color);

	/* Add labels, buttons to window */
	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_box_pack_start(GTK_BOX(vbox), valign_content, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), halign_manual, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), halign_button, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(valign_content), content_box);
	gtk_container_add(GTK_CONTAINER(halign_manual), manual_box);
	gtk_container_add(GTK_CONTAINER(halign_button), hbox);

	gtk_box_pack_start(GTK_BOX(content_box), lb_content, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(manual_box), lb_manual, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), btn_restart, TRUE, TRUE, 20);
	gtk_container_add(GTK_CONTAINER(btn_restart), restart_eventbox);
	gtk_container_add(GTK_CONTAINER(restart_eventbox), lb_restart);

	gtk_box_pack_start(GTK_BOX(hbox), btn_postpone, TRUE, TRUE, 20);
	gtk_container_add(GTK_CONTAINER(btn_postpone), postpone_eventbox);
	gtk_container_add(GTK_CONTAINER(postpone_eventbox), lb_postpone);

	/* Send mouse and keyboard signal */
	g_signal_connect(G_OBJECT(btn_restart), "clicked", G_CALLBACK(btn_restart_clicked), NULL);
	g_signal_connect(G_OBJECT(btn_postpone), "clicked", G_CALLBACK(btn_postpone_clicked), NULL);
	g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(key_press_handle), NULL);

	gtk_widget_show_all(window);

	/* Start countdown 60 seconds when show window */
	timer_restart_id = g_timeout_add_seconds(1, (GSourceFunc)gb_restart_automatic, (gpointer) lb_content);

	gtk_main();
	return 0;
}

int get_info(const char *cmd)
{
	int result;
	FILE *fp;
	char path[100];

	/* Open the command for reading. */
	fp = popen(cmd, "r");
	if(fp == NULL) {
		printf("Failed to run command\n");
		return 0;
	}
	/* Read the output a line at a time - output it. */
	while(fgets(path, sizeof(path) - 1, fp) != NULL)
		result = atoi(path);

	/* close */
	pclose(fp);
	return result;
}

void restart()
{
#ifdef DEBUG
	g_print("Restart\n");
#endif
	system("reboot");
	gtk_main_quit();
}

void postpone(GtkWidget *widget, gpointer data)
{
	if(timer_restart_id) {
		g_source_remove(timer_restart_id);
		restart_timeout = RESTART_TIMEOUT;
		markuptitle = g_markup_printf_escaped(format_content, CONTENT);
		gtk_label_set_markup(GTK_LABEL(lb_content), markuptitle);
		g_free(markuptitle);
	}
#ifdef DEBUG
	g_print("Remind in 10 minutes\n");
#endif
	gtk_widget_hide(GTK_WIDGET(window));
	timer_postpone_id = g_timeout_add_seconds(REMIND_TIMEOUT, (GSourceFunc)gb_postpone, (gpointer) widget);
}

void set_window_size(GtkWidget *widget)
{
	int ret;
	unsigned int vdi_connection = 0;

	ret = system("branch-check | grep 1 > /dev/null 2>&1");
	if(WIFEXITED(ret))
		vdi_connection = WEXITSTATUS(ret);
	if(vdi_connection == 0)
		gtk_widget_set_size_request(GTK_WIDGET(widget), screen_width, screen_height);
	else
		gtk_window_fullscreen(GTK_WINDOW(widget));
}

void btn_restart_clicked(GtkWidget *widget, gpointer data)
{
	restart();
}

void btn_postpone_clicked(GtkWidget *widget, gpointer data)
{
	postpone(widget, data);
}

gboolean key_press_handle(GtkWidget *widget, GdkEventKey *kevent, gpointer data)
{
	if(kevent->type == GDK_KEY_PRESS) {
		if(kevent->keyval == 'r') {
#ifdef DEBUG
			g_message("%d, %c;", kevent->keyval, kevent->keyval);
#endif
			restart();
		} else if(kevent->keyval == 'p') {
#ifdef DEBUG
			g_message("%d, %c;", kevent->keyval, kevent->keyval);
#endif
			postpone(widget, data);
		}
	}

	return FALSE;
}

gboolean gb_postpone(GtkWidget *widget)
{
	if(widget == NULL) return FALSE;

	if(timer_postpone_id) {
		g_source_remove(timer_postpone_id);
		gtk_widget_show(GTK_WIDGET(window));
		timer_restart_id = g_timeout_add_seconds(1, (GSourceFunc)gb_restart_automatic, (gpointer) widget);
	}

	return TRUE;
}

gboolean gb_restart_automatic(GtkWidget *widget)
{
	gchar buf[256];

	if(widget == NULL) return FALSE;

	if(restart_timeout > 2)
		snprintf(buf, 255, content_msg1, --restart_timeout);
	else
		snprintf(buf, 255, content_msg2, --restart_timeout);
	markuptitle = g_markup_printf_escaped(format_content, buf);
	gtk_label_set_markup(GTK_LABEL(lb_content), markuptitle);
	g_free(markuptitle);

	if(restart_timeout < 1) {
#ifdef DEBUG
		g_print("Restart Automatically\n");
#endif
		system("reboot");
		gtk_main_quit();
		if(timer_restart_id) {
			g_source_remove(timer_restart_id);
		}
	}

	return TRUE;
}