/************************************************
 * TzcsKeyboard.c
 *
 * Copyright 2016 TOSHIBA PLATFORM SOLUTION CORPORATION
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2017/01/14  TSDV
 *  - New
 *  -  
 *  2017/02/16  TOPS
 *  - I18N
 *
 ************************************************/
#ifdef PCDEBUG
#include "TzcsUtil.h"
#else
#include <tzcslib/TzcsUtil.h>
#endif
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tzcsI18N.h"
#include <locale.h>

/*--------------------------------------------------------------------
 * Global Definitions
 *------------------------------------------------------------------*/
#define WIN_BORDER          10
// #define VAL_KEYBOARD_TYPE   0x0045  // Keyboard type
// #define DEBUG               1
void label_color_white(GtkWidget *widget);
void widget_color_blue(GtkWidget *widget);
void label_modify_depend_lang (GtkWidget *label, const char *font, int size,
        int *lwidth, int *lheight);

GtkWidget *LeftPaneTreeView;
/*--------------------------------------------------------------------
 * Resouces
 *------------------------------------------------------------------*/
#ifdef PCDEBUG
char *keyboard_list_path = "keyboard_list";
#else
char *keyboard_list_path = "/usr/share/controlpanel/resource/keyboard_list";
#endif
static char *confirmLabel = N_("Edit keyboard layout settings.");

/*--------------------------------------------------------------------
 * Commands
 *------------------------------------------------------------------*/
const char *getcurrentkeyboardlayout = "setxkbmap -print | awk -F'+' '/xkb_symbols/ {print $2}'"; //Get current keyboard layout

/*--------------------------------------------------------------------
 * Global Data
 *------------------------------------------------------------------*/
char *current_kbd = "";
char new_kbd[256] = "";
char new_kbd_code[256] = "";
char *countries_name[100];
char *countries_code[100];
int countries_num = 0;
enum {
    COLUMN_ITEM,
    N_COLUMNS
};

/*-------------------------------------------------------------------
 * Function   : find_index
 *------------------------------------------------------------------*/
/**
 * Find the index of string in the array of string
 * @param     char *arr[]
 *            int size
 *            char *value
 *
 * @return    int
 * @note      nothing
 */
/*------------------------------------------------------------------*/
static int find_index (char *arr[], int size, char *value) {
    int index = 0;
    while (index < size && strcmp(arr[index],value) != 0) {
        ++index;
    }
    return (index == size ? -1 : index);
}

/*-------------------------------------------------------------------
 * Function   : get_str_from_command
 *------------------------------------------------------------------*/
/**
 * Get value of string from the command
 * @param     char *cmd
 *
 * @return    char *
 * @note      nothing
 */
/*------------------------------------------------------------------*/
static char *get_str_from_command (const char *cmd) {
    FILE *fp;
    char path[100];
    char *temp = "";
    /* Open the command for reading. */
    fp = popen (cmd, "r");
    if (fp == NULL) {
        g_print ("Failed to run command #001\n");
    }
    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof (path) - 1, fp) != NULL) {
        temp = path;
    }
    /* close */
    pclose(fp);
    return strdup(temp);
}

/*-------------------------------------------------------------------
 * Function   : trim
 *------------------------------------------------------------------*/
/**
 * Get value of string from the command
 * @param     char *str
 *
 * @return    char *
 * @note      nothing
 */
/*------------------------------------------------------------------*/
static char *trim (char *str) {
    char *p;
    if ((p = strchr (str, '\n')) != NULL) {
        *p = '\0';
    }
    return str;
}

static void keyboard_data_set (void)
{
    current_kbd = trim(get_str_from_command(getcurrentkeyboardlayout));
    if (strcmp(current_kbd, new_kbd_code) != 0 || strlen(new_kbd_code) != 0) {
#ifndef PCDEBUG
        ULONG idx = find_index (countries_name, countries_num, new_kbd);
        set_hci_value (VAL_KEYBOARD_TYPE, &idx);	/* 2017/01/14 TSDV */
#endif
        char command[512] = "setxkbmap ";
        strcat (command, new_kbd_code);
        strcat (command, "\n");
#ifndef PCDEBUG
        system(command);
#else
        g_print ("Command executed: %s\n", command);
#endif
    }
    gtk_main_quit();
}


/*-------------------------------------------------------------------
 * Function   : get_supported_countries
 *------------------------------------------------------------------*/
/**
 * Get list of supported keyboard layouts
 * @param     non
 *
 * @return    non
 * @note      nothing
 */
/*------------------------------------------------------------------*/
static void get_supported_countries () {
    FILE *fp = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int counter = 0;

    fp = fopen (keyboard_list_path, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        int index = 0;
        char *token;
        while ((token = strsep (&line, ";"))) {
            if (index == 0) {
                index++;
                countries_name[counter] = (char*)malloc(sizeof(token));
                strcpy (countries_name[counter], trim (token));
            } else if (index == 1) {
                index++;
                countries_code[counter] = (char*)malloc(sizeof(token));
                strcpy (countries_code[counter], trim (token));
            }
        }
        counter++;
    }
    countries_num = counter;

    fclose(fp);

#ifdef DEBUG
    int i = 0;
    g_print ("List supported countries:\n");
    for (i = 0; i < countries_num; ++i) {
        g_print ("%s -- %s\n", countries_name[i], countries_code[i]);
    }
#endif
}

static void cursor_treeview(GtkTreeView *treeview, gpointer user_data)
{
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *name;
    int i;

    selection = gtk_tree_view_get_selection(treeview);
    if (!selection)
        return;
    if (!gtk_tree_selection_get_selected(selection, &model, &iter))
        return;

    gtk_tree_model_get(model, &iter, COLUMN_ITEM, &name, -1);
    for (i = 0; i < countries_num; i++) {
        if (!strcmp((const char *)name, countries_name[i])) {
            strcpy (new_kbd, countries_name[i]);
            strcpy (new_kbd_code, countries_code[i]);
        }
    }
    g_free (name);
}

static void arrow_key(GtkTreeView *treeview, GtkMovementStep arg1,
        gint arg2, gpointer user_data)
{
#define LEFT (arg1 == 1 && arg2 == -1)
    if (!LEFT)
        return;

    gtk_widget_grab_focus(LeftPaneTreeView);
}

static GtkWidget *create_treeview()
{
    GtkWidget *treeview, *scroll;
    GtkListStore *liststore;
    GtkTreeIter iter;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    GtkTreePath *path;
    int i, idx;

    current_kbd = trim (get_str_from_command (getcurrentkeyboardlayout));

    liststore = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING);
    treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));
    gtk_widget_set_name (treeview, "KeyboardTreeView");

    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
    g_object_unref (liststore);

    g_signal_connect (GTK_OBJECT (treeview), "cursor-changed",
            G_CALLBACK (cursor_treeview), NULL);
    g_signal_connect (GTK_OBJECT (treeview), "move-cursor",
            G_CALLBACK (arrow_key), NULL);

    column = gtk_tree_view_column_new ();
    gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
            GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN(column), 252);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, renderer, FALSE);
    gtk_tree_view_column_set_attributes (column, renderer, "text",
            COLUMN_ITEM, NULL);
    g_object_set(renderer, "ypad", 10, NULL);
    g_object_set(renderer, "xpad", 10, NULL);
    g_object_set(renderer, "font-desc",
            pango_font_description_from_string("Sans 15"), NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

    scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scroll), treeview);

    for (i = 0; i < countries_num; i++) {
        gtk_list_store_append (liststore, &iter);
        gtk_list_store_set (liststore, &iter,
                COLUMN_ITEM, countries_name[i], -1);
        if (!strcmp (countries_code[i], current_kbd)) {
            gtk_tree_selection_select_iter (
                    gtk_tree_view_get_selection((GtkTreeView *)treeview),
                    &iter);
            idx = i;
        }
    }
    path = gtk_tree_path_new_from_indices(idx, -1);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(treeview), path, NULL, TRUE, 0, 0);

    return scroll;
}


/*-------------------------------------------------------------------
 * Function   : main
 *------------------------------------------------------------------*/
/**
 * TZCS Control Panel Time Zone
 * @param     int argc
 *            char *argv[]
 *
 * @return    EXIT Code
 * @note      nothing
 */
/*------------------------------------------------------------------*/
GtkWidget *TzcsKeyboard(void) {
    GtkWidget *vbox;
    GtkWidget *frame;
    GtkWidget *label, *klabel, *flabel;
    GtkWidget *table;
    GtkWidget *rbox, *button;
    int lwidth, lheight, xbase, ybase;

    gtk_rc_parse_string ("widget \"*.KeyboardTreeView\" style \"TZCS2\"\n");

    get_supported_countries();

    /* Create PackingBox */
    vbox = gtk_vbox_new (FALSE, WIN_BORDER);

    /* Create frame */
    frame = gtk_frame_new (_("Keyboard Layout :"));
    flabel = gtk_frame_get_label_widget(GTK_FRAME(frame));
    gtk_widget_modify_font (GTK_WIDGET(flabel),
            pango_font_description_from_string("Sans 14"));

    table = gtk_table_new(568, 708, TRUE);
    xbase = 24;
    ybase = 20;
    klabel = gtk_label_new (_("Keyboard"));
    label_modify_depend_lang (klabel, "Sans", 34, &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(table), klabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
    ybase = ybase + lheight + 24;

    label = gtk_label_new (_(confirmLabel));
    label_modify_depend_lang (label, "Sans", 18, &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(table), label,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
    ybase = ybase + lheight + 24;

    rbox = create_treeview();
    gtk_container_add(GTK_CONTAINER(frame), rbox);
    gtk_table_attach(GTK_TABLE(table), frame,
            xbase , xbase + 400,
            ybase, ybase + 235,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    button = gtk_button_new_with_label(_("OK"));
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
            GTK_SIGNAL_FUNC(keyboard_data_set), NULL);
    label_color_white(gtk_bin_get_child((GtkBin *)button));
    widget_color_blue(button);
    ybase = ybase + 250;
    xbase = xbase + 430;
    gtk_table_attach(GTK_TABLE(table), button,
            xbase, xbase + 100,
            ybase, ybase + 32,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);

    return vbox;
}
