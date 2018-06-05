#include <gtk/gtk.h>
#include <stdio.h>
#include <time.h>
#include <glib.h>
#include "url.h"
#define time_out 5000
char buffer[256];
char *first_frame_color = "#000000";
const char *format ="<span font_desc=\"13.5\" color=\"#ffffff\">\%s</span>" ;//style for text of index label
const char *setlayer_popup ="xdotool windowactivate `xdotool search --name \"gtkpopup\" | tail -1` &";
const char *setlayer_popup1 ="xdotool windowactivate `xdotool search --name \"gtkpopup\" | head -1` &";

gboolean time_destroy_popup(gpointer data_widget) {
	time_t curtime;
	struct tm *loctime;
	curtime =  time(NULL);
	loctime = localtime(&curtime);
	strftime(buffer,256, "%T", loctime);
	gtk_widget_destroy(GTK_WIDGET(data_widget));
	return TRUE;
}
static int show_popup(char *image_url, char *text){
	GtkWidget *popup_window;
	GtkWidget *image;
	GtkWidget *popup_info;
	GtkWidget *hbox;
	GdkColor color;
	char *markuptitle;
	popup_window = gtk_window_new(GTK_WINDOW_POPUP);
	gdk_color_parse(first_frame_color,&color);	
	gtk_window_set_keep_above (GTK_WINDOW(popup_window), TRUE);
	g_timeout_add(time_out,(GSourceFunc) time_destroy_popup,(gpointer) popup_window);
	gtk_widget_modify_bg(popup_window, GTK_STATE_NORMAL, &color);
	gtk_window_set_decorated(GTK_WINDOW(popup_window),FALSE);
	gtk_window_set_position(GTK_WINDOW(popup_window), GTK_WIN_POS_CENTER );
	gtk_widget_set_size_request(GTK_WIDGET(popup_window),main_window_width,121);
	g_signal_connect(G_OBJECT(popup_window), "destroy", G_CALLBACK (gtk_main_quit), NULL);	
	hbox = gtk_hbox_new(FALSE,20);
	image = gtk_image_new_from_file(image_url);
	popup_info = gtk_label_new(NULL);
	markuptitle =  g_markup_printf_escaped(format,text); 
	gtk_label_set_markup(GTK_LABEL(popup_info),markuptitle);                                                                                 
	g_free(markuptitle);
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,20);
	gtk_box_pack_start(GTK_BOX(hbox),popup_info,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(popup_window), hbox);
	gtk_widget_show_all(popup_window);
	system(setlayer_popup);
	system(setlayer_popup1);
	gtk_main();
	return 1;
}
int main(int argc,char *argv[]){
	gtk_init(&argc, &argv);
	show_popup(warning,argv[1]);
	return 1;

}
