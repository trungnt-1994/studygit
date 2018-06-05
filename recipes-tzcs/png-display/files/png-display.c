/****************************************************************
 * Copyright (C) 2017 TOSHIBA CORPORATION. All rights reserved. *
 ****************************************************************
 *
 * @file:  png-display.c
 * @brief Display a png image file in fullscreen mode.
 *
 * Author: Tam. Tran Van (tam.tranvan@toshiba-tsdv.com)
 * @bugs Report to author.
 *
 * Created on November 21, 2017, 4:39 PM
 ****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

int get_screen_size(int *width, int *height);
gboolean resize_window(/*GtkWidget *widget, GdkEvent *event, */gpointer data);
#define BLACK "#000000"
//#define DEBUG

int width = 0, height = 0;
int now_width = 0, now_height = 0;

int get_screen_size(int *width, int *height)
{
    Display *dpy;
    int screen;

    /* Connect to X-Server, get root window id */
    dpy= XOpenDisplay(NULL);
    screen = DefaultScreen(dpy);

    /* Get current resolution */
    *width = DisplayWidth(dpy, screen);
    *height = DisplayHeight(dpy, screen);
    XCloseDisplay(dpy);
    return 0;
}

gboolean resize_window(/*GtkWidget *widget, GdkEvent *event, */gpointer data)
{
    GtkWidget *window = g_object_get_data(G_OBJECT(data), "window");
    GtkWidget *image = g_object_get_data(G_OBJECT(data), "image");
    get_screen_size(&width, &height);
    gtk_window_move(GTK_WINDOW(window), 0, 0);
    if ((width != now_width) || (height != now_height)) {
#ifdef DEBUG
        printf("screen width = %i, screen height = %i \n", width, height);
#endif
        gtk_window_resize(GTK_WINDOW(window), width, height);
        gtk_widget_set_size_request(GTK_WIDGET(image), width, height);
        gtk_widget_show_all(window);
        now_width = width; now_height = height;
    }
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *image;
    GdkColor bg_color;
    GError *error = NULL;

    gdk_color_parse(BLACK, &bg_color);

    /* Initialize */
    gtk_init(&argc, &argv);

    /* Creat Widgets */
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_move(GTK_WINDOW(window), 0, 0);
    get_screen_size(&width, &height);
    gtk_window_resize(GTK_WINDOW(window), width, height);
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &bg_color);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);
    gtk_window_set_focus_on_map(GTK_WINDOW(window), TRUE);

    image = gtk_image_new_from_file(argv[1]);
    if (image == NULL) {
#ifdef DEBUG
        g_printerr("Could not open file \"%s\"\n", argv[1]);
#endif
        return 1;
    }
    gtk_widget_set_size_request(GTK_WIDGET(image), width, height);
    gtk_container_add (GTK_CONTAINER(window), image);

    /* Timer updates window size */
    g_object_set_data(G_OBJECT(image), "window", window);
    g_object_set_data(G_OBJECT(image), "image", image);
    g_timeout_add(500, resize_window, (gpointer)image);

    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();

    return 0;
}
