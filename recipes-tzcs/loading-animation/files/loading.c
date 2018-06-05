#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

int get_screen_size(int *width, int *height);
gboolean resize_window(/*GtkWidget *widget, GdkEvent *event, */gpointer data);
#define WHITE "#ffffff"
#define FILE_ST "/tmp/loading.txt"
#define BUFF_SIZE 10
#define TIME_OUT 60
//#define DEBUG 1

int width = 0, height = 0;
int now_width = 0, now_height = 0;
int timer = TIME_OUT;

int get_screen_size(int *width, int *height)
{
    Display *dpy;
    int screen;

    /* Connect to X-Server, get root window id */
    dpy= XOpenDisplay(NULL);
//    screen = DefaultScreen(dpy);

    /* Get current resolution */
//    *width = DisplayWidth(dpy, screen);
//    *height = DisplayHeight(dpy, screen);

    XRRScreenResources *screens = XRRGetScreenResources(dpy, DefaultRootWindow(dpy));
    XRRCrtcInfo *info = NULL;
    RROutput primary_id = XRRGetOutputPrimary(dpy, DefaultRootWindow(dpy));

    int i = 0, is_found = FALSE;
    while (i < screens->ncrtc && is_found == FALSE) {
        info = XRRGetCrtcInfo(dpy, screens, screens->crtcs[i]);
        if (info->width != 0 && info->height != 0) {
#ifdef DEBUG
            printf("%dx%d\n", info->width, info->height);
#endif
            // Find out the first screen resolution
            *width = info->width;
            *height = info->height;
            is_found = TRUE;
        }
        i++;
    }
#ifdef DEBUG
        printf("===========\n");
#endif
    XRRFreeCrtcInfo(info);
    XRRFreeScreenResources(screens);

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
#else
        gtk_window_resize(GTK_WINDOW(window), width, height);
        gtk_widget_set_size_request(GTK_WIDGET(image), width, height);
        gtk_widget_show_all(window);
#endif
        now_width = width; now_height = height;
    }
    return TRUE;
}

gboolean read_value(gpointer data)
{
    char buff[BUFF_SIZE];
    memset(buff, '\0', sizeof(buff));
    FILE *fp = NULL;
    fp = fopen(FILE_ST, "a+");
    if (fp != NULL) {
        fgets(buff, BUFF_SIZE, fp);
#ifdef DEBUG
        printf("Value: %s\n", buff);
#endif
        if (strncmp(buff, "0", 1) == 0 || timer == 0) {
            freopen(FILE_ST, "w", fp);
            fputs("1", fp);
            fclose(fp);
            gtk_main_quit();
        }
    }
    fclose(fp);
    timer--;
    return TRUE;
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    GtkWidget *window;
    GtkWidget *image;
    GdkColor bg_color;
    gdk_color_parse(WHITE, &bg_color);
    /* Creat Widgets */
    window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_move(GTK_WINDOW(window), 0, 0);
    get_screen_size(&width, &height);

    if (width != 0 && height != 0) {
        gtk_window_resize(GTK_WINDOW(window), width, height);
    }
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &bg_color);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);
    gtk_window_set_focus_on_map(GTK_WINDOW(window), TRUE);
    image = gtk_image_new_from_file ("/usr/share/loading-animation/loading.gif");
    gtk_widget_set_size_request(GTK_WIDGET(image), width, height);
    gtk_container_add (GTK_CONTAINER(window), image);

    /* Timer updates window size */
    g_object_set_data(G_OBJECT(image), "window", window);
    g_object_set_data(G_OBJECT(image), "image", image);
    g_timeout_add(1000, resize_window, (gpointer)image);

    /* Timer reading value of loading.txt then quit main loop */
    g_timeout_add(500, read_value, NULL);

#ifndef DEBUG
    gtk_widget_show_all(GTK_WIDGET(window));
//--add Mouse cursor is hidden.
    GdkWindow* win = gtk_widget_get_window(window);
    GdkCursor* cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
    gdk_window_set_cursor(win, cursor);
//--end Mouse cursor is hidden.
#endif

    gtk_main();
}

