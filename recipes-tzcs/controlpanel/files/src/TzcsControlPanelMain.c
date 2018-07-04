/************************************************
 * TzcsControlPanelMain.c
 *
 * Copyright 2016-2017 TOSHIBA PLATFORM SOLUTION CORPORATION
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2016/09/12  TOPS
 *  - New
 *  -  
 *  2017/02/16  TOPS
 *  - I18N
 *
 ************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <gtk/gtk.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <xfce-randr.h>

#define __MAIN__   /* this is main file */

#include "tzcsI18N.h"
#include <locale.h>
#include "common.h"

/*--------------------------------------------------------------------
 * Global Definitions
 *------------------------------------------------------------------*/
#define WIN_TITLE_MAIN		_("Control Panel")

#define EULA			N_("EULA")
#define TIMEZONE		N_("TimeZone")
#define DISPLAY			N_("Display")
#define KEYBOARD		N_("Keyboard")
#define TOUCHPAD		N_("Touch Pad")
#define ABOUT			N_("About")
GtkWidget *LeftPaneTreeView;

/*--------------------------------------------------------------------
 * Resouces
 *------------------------------------------------------------------*/
#if PCDEBUG
#define ICONPATH "resource"
#else
#define ICONPATH "/usr/share/controlpanel/resource"
#endif
#define ICON_T ICONPATH "/ToshibaLogo@1x.png"
#define ICON_EULA ICONPATH "/EULA@1x.png"
#define ICON_EULA2 ICONPATH "/EULA_s@1x.png"
#define ICON_TIME ICONPATH "/TimeZone@1x.png"
#define ICON_TIME2 ICONPATH "/TimeZone_s@1x.png"
#define ICON_DISP ICONPATH "/Display@1x.png"
#define ICON_DISP2 ICONPATH "/Display_s@1x.png"
#define ICON_KEY ICONPATH "/Keyboard@1x.png"
#define ICON_KEY2 ICONPATH "/Keyboard_s@1x.png"
#define ICON_ABT ICONPATH "/About@1x.png"
#define ICON_ABT2 ICONPATH "/About_s@1x.png"
#define ICON_TPD ICONPATH "/TouchPad@1x.png"
#define ICON_TPD2 ICONPATH "/TouchPad_s@1x.png"

GtkWidget *main_window;

enum {
    COLUMN_IMAGE,
    COLUMN_ITEM,
    N_COLUMNS
};

GtkWidget *TzcsCPTimeZone(void);
GtkWidget *TzcsKeyboard(void);
GtkWidget *TzcsDisplay(void);
GtkWidget *TzcsAbout(void);
GtkWidget *TzcsTouchpad(void);
GtkWidget *TzcsEULA(void);
void Widget_color_white(GtkWidget *widget);

int number_items;   /* number of items for main window */
XfceRandr *xfce_randr;

static struct item_detail {
    const gchar *item_name;
    const gchar *logo_file;
    const gchar *logo_file2;
    GtkWidget *(*func)(void);
} *item_detail;

/* toshiba logo */
static GtkWidget *create_logo_hbox ()
{
    GtkWidget *hbox;
    GtkWidget *image;

    /* image */
    image = gtk_image_new_from_file (ICON_T);

    /* logo image hbox */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

    return hbox;
}

static GtkWidget *create_item_detail_note ()
{
    GtkWidget *widget;
    GtkWidget *notebook;

    notebook = gtk_notebook_new ();
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);
    gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook), FALSE);

    for (int i = 0; i < number_items; i++) {
        widget = (item_detail[i].func) ();
        gtk_notebook_append_page (GTK_NOTEBOOK (notebook), widget, NULL);
    }

    return notebook;
}

static void cursor_treeview (GtkTreeView *treeview, gpointer user_data)
{
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkWidget *notebook;
    GdkPixbuf *icon;
    int i, lcnt;

    notebook = (GtkWidget *)user_data;

    selection = gtk_tree_view_get_selection(treeview);
    if (!selection)
        return;
    if (!gtk_tree_selection_get_selected(selection, &model, &iter))
        return;

    /* select notebook */
    gchar *name;
    lcnt = 0;
    gtk_tree_model_get(model, &iter, COLUMN_ITEM, &name, -1);
    for (i = 0; i < number_items; i++) {
        if (!strcmp((const char *)name, _(item_detail[i].item_name))) {
            gtk_notebook_set_current_page(
                    GTK_NOTEBOOK (notebook), (gint)i);
            lcnt = i;
            break;
        }
    }

    gtk_list_store_clear (GTK_LIST_STORE (model));
    for (i = 0; i < number_items; i++) {
        if (i == lcnt)
            icon = gdk_pixbuf_new_from_file (
                    item_detail[i].logo_file2, NULL);
        else
            icon = gdk_pixbuf_new_from_file (
                    item_detail[i].logo_file, NULL);
        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                COLUMN_IMAGE, icon,
                COLUMN_ITEM, _(item_detail[i].item_name),
                -1);
        if (i == lcnt)
            gtk_tree_selection_select_iter (selection, &iter);
    }
    g_free (name);
}

/* arrow key, etc... */
static void move_cursor (GtkTreeView *treeview, GtkMovementStep arg1,
        gint arg2, gpointer user_data)
{
#define UP (arg1 == 3 && arg2 == -1)
#define DOWN (arg1 == 3 && arg2 == 1)
#define LEFT (arg1 == 1 && arg2 == -1)
#define RIGHT (arg1 == 1 && arg2 == 1)

    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (RIGHT) {
        gtk_widget_child_focus((GtkWidget *)user_data, GTK_TEXT_DIR_NONE);
        return;
    }
    if (!UP && !DOWN)
        return;
    selection = gtk_tree_view_get_selection (treeview);
    if (!selection)
        return;
    if (!gtk_tree_selection_get_selected (selection, &model, &iter))
        return;
    gchar *name;
    gtk_tree_model_get (model, &iter, COLUMN_ITEM, &name, -1);
    for (int i = 0; i < number_items; i++) {
        if (!strcmp ((const char *)name, _(item_detail[i].item_name))) {
            GtkTreePath *path;
            if ((i == 0 && UP) || (i == number_items -1 && DOWN))
                break;
            if (UP)
                path = gtk_tree_path_new_from_indices (i - 1, -1);
            else
                path = gtk_tree_path_new_from_indices (i + 1, -1);
            gtk_tree_selection_select_path (selection, path);
            gtk_tree_path_free (path);
            break;
        }
    }
    g_free (name);
}

/* Enter key */
static gboolean select_cursor_row(GtkTreeView *treeview, gboolean arg1,
        gpointer user_data)
{
    gtk_widget_child_focus ((GtkWidget *)user_data, GTK_TEXT_DIR_NONE);
    return TRUE;
}

static GtkWidget *create_item_treeview(GtkWidget *notebook)
{
    GtkWidget *treeview;
    GtkListStore *liststore;
    GtkTreeIter iter;
    GdkPixbuf *icon;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    int i;

    liststore = gtk_list_store_new (N_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(liststore));
    gtk_widget_set_name (treeview, "ControlPanelTreeView");

    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
    g_object_unref (liststore);

    g_signal_connect (GTK_OBJECT (treeview), "cursor-changed",
            G_CALLBACK (cursor_treeview), (gpointer)notebook);
    g_signal_connect(GTK_OBJECT (treeview), "move-cursor",
            G_CALLBACK (move_cursor), (gpointer)notebook);
    g_signal_connect_after (GTK_OBJECT (treeview), "select-cursor-row",
            G_CALLBACK (select_cursor_row), (gpointer)notebook);

    column = gtk_tree_view_column_new ();
    gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
            GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 252);

    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (column, renderer, FALSE);
    gtk_tree_view_column_set_attributes (column, renderer, "pixbuf",
            COLUMN_IMAGE, NULL);
    g_object_set(renderer, "ypad", 15, NULL);
    g_object_set(renderer, "xpad", 15, NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, renderer, FALSE);
    gtk_tree_view_column_set_attributes (column, renderer, "text",
            COLUMN_ITEM, NULL);

    g_object_set(renderer, "font-desc",
            pango_font_description_from_string("Sans 15"), NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    for (i = 0; i < number_items; i++) {
        icon = gdk_pixbuf_new_from_file (item_detail[i].logo_file,
                NULL);
        gtk_list_store_append (liststore, &iter);
        gtk_list_store_set (liststore, &iter, COLUMN_IMAGE, icon,
                COLUMN_ITEM, _(item_detail[i].item_name), -1);
        if (!i)
            gtk_tree_selection_select_iter (
                    gtk_tree_view_get_selection((GtkTreeView *)treeview),
                    &iter);
    }
    return treeview;
}

/*-------------------------------------------------------------------
 * Function   : CloseAppWindow
 *------------------------------------------------------------------*/
/**
 * Window Close signal Callback
 * @param     GtkWidget *widget
 *            gpointer gdata
 *
 * @return    boolean
 * @note      nothing
 */
/*------------------------------------------------------------------*/
gint CloseAppWindow (GtkWidget * widget, gpointer gdata)
{
#ifdef DEBUG
    g_print ("TZCS ControlPanel Main Quitting ... \n");
#endif
    gtk_main_quit ();

    return (FALSE);
}

static gboolean KeyPressWindow (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    if (event->keyval == GDK_KEY_Escape)
        gtk_main_quit ();
    return FALSE;
}

/*-------------------------------------------------------------------
 * Function   : main
 *------------------------------------------------------------------*/
/**
 * TZCS Control Panel MAIN
 * @param     int argc
 *            char *argv[]
 *
 * @return    EXIT Code
 * @note      nothing
 */
/*------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
    GdkDisplay *display;

    GtkWidget *logo_hbox, *hbox, *vbox;
    GtkWidget *item_treeview, *item_detail_note;
    int status;
    pid_t pid;

    /* I18N */
    setlocale (LC_ALL, "");
    textdomain ("tzcs-control-panel");

    if (touchpad_type == SYNAPTIC) {
        number_items = 5;
        item_detail = calloc(number_items, sizeof *item_detail);
        item_detail[0] = (struct item_detail) {.item_name = DISPLAY, .logo_file = ICON_DISP, .logo_file2 = ICON_DISP2, .func = TzcsDisplay};
        item_detail[1] = (struct item_detail) {.item_name = KEYBOARD, .logo_file = ICON_KEY, .logo_file2 = ICON_KEY2, .func = TzcsKeyboard};
        item_detail[2] = (struct item_detail) {.item_name = TIMEZONE, .logo_file = ICON_TIME, .logo_file2 = ICON_TIME2, .func = TzcsCPTimeZone};
        item_detail[3] = (struct item_detail) {.item_name = EULA, .logo_file = ICON_EULA, .logo_file2 = ICON_EULA2, .func = TzcsEULA};
        item_detail[4] = (struct item_detail) {.item_name = ABOUT, .logo_file = ICON_ABT, .logo_file2 = ICON_ABT2, .func = TzcsAbout};
    } else {
        number_items = 6;
        item_detail = calloc(number_items, sizeof *item_detail);
        item_detail[0] = (struct item_detail) {.item_name = DISPLAY, .logo_file = ICON_DISP, .logo_file2 = ICON_DISP2, .func = TzcsDisplay};
        item_detail[1] = (struct item_detail) {.item_name = KEYBOARD, .logo_file = ICON_KEY, .logo_file2 = ICON_KEY2, .func = TzcsKeyboard};
        item_detail[2] = (struct item_detail) {.item_name = TIMEZONE, .logo_file = ICON_TIME, .logo_file2 = ICON_TIME2, .func = TzcsCPTimeZone};
        item_detail[3] = (struct item_detail) {.item_name = TOUCHPAD, .logo_file = ICON_TPD, .logo_file2 = ICON_TPD2, .func = TzcsTouchpad};
        item_detail[4] = (struct item_detail) {.item_name = EULA, .logo_file = ICON_EULA, .logo_file2 = ICON_EULA2, .func = TzcsEULA};
        item_detail[5] = (struct item_detail) {.item_name = ABOUT, .logo_file = ICON_ABT, .logo_file2 = ICON_ABT2, .func = TzcsAbout};
    }

    /* Init : Locale setting */
    gtk_set_locale ();
    /* Init : Options */
    gtk_init (&argc, &argv);

    { /* init XfceRandr instance */
        display = gdk_display_get_default();
        xfconf_init(NULL);
        xfce_randr = xfce_randr_new(display, NULL);
    }

    gtk_rc_parse_string (
            "style \"TZCS\" { \n"
            "base[NORMAL] = \"#EFF4F8\"\n"
            "base[SELECTED] = \"#3076BC\"\n"
            "base[ACTIVE] = \"#3076BC\"\n"
            "}\n"
            "style \"TZCS2\" { \n"
            "base[NORMAL] = \"#EFF4F8\"\n"
            "base[SELECTED] = \"#8FB4DA\"\n"
            "base[ACTIVE] = \"#8FB4DA\"\n"
            "}\n"
            "style \"TZCS3\" { \n"
            "bg[NORMAL] = \"#EFF4F8\"\n"
            "bg[SELECTED] = \"#3076BC\"\n"
            "bg[ACTIVE] = \"#EFF4F8\"\n"
            "bg[PRELIGHT] = \"#EFF4F8\"\n"
            "}\n"
            "widget \"*.ControlPanelTreeView\" style \"TZCS\"\n");

    /* Create TOP Level Window */
    main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position (GTK_WINDOW (main_window),
            GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_set_size_request (main_window, 960, 568);
    gtk_window_set_title (GTK_WINDOW (main_window), WIN_TITLE_MAIN);
    gtk_window_set_resizable (GTK_WINDOW (main_window), FALSE);	/* disable resize */
    gtk_signal_connect (GTK_OBJECT (main_window), "delete_event",
            GTK_SIGNAL_FUNC (CloseAppWindow), NULL);
    gtk_signal_connect (GTK_OBJECT (main_window), "key_press_event",
            GTK_SIGNAL_FUNC (KeyPressWindow), NULL);
    Widget_color_white (GTK_WIDGET(main_window));

    /* toshiba logo */
    logo_hbox = create_logo_hbox ();

    /* item detail */
    item_detail_note = create_item_detail_note ();

    /* treeview */
    item_treeview = create_item_treeview (item_detail_note);

    LeftPaneTreeView = item_treeview;

    vbox = gtk_vbox_new (FALSE, 0);
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX(hbox), item_treeview, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(hbox), item_detail_note, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(vbox), logo_hbox, FALSE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    /* show all and execute */
    gtk_widget_show_all (main_window);
    gtk_main ();
    /******************/
    do
    {
        pid = waitpid (0, &status, 0);
#ifdef DEBUG
        printf ("pid=%d , status= %d\n", pid, status);
        perror ("wait");
#endif
        usleep (100000);
    }
    while (pid >= 0);		/* wait sub all process */
    return 0;
}
