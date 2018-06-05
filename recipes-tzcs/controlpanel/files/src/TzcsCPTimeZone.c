/************************************************
 * TzcsCPTimeZone.c
 *
 * Copyright 2016-2017 TOSHIBA PLATFORM SOLUTION CORPORATION
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2016/09/08  TOPS
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
#define MAX_REGION 20
void label_color_white(GtkWidget *widget);
void widget_color_blue(GtkWidget *widget);
void label_modify_depend_lang (GtkWidget *label, const char *font, int size,
        int *lwidth, int *lheight);

GtkWidget *LeftPaneTreeView;
enum {
    COLUMN_ITEM,
    N_COLUMNS
};

/*--------------------------------------------------------------------
 * Resouces
 *------------------------------------------------------------------*/
#ifdef PCDEBUG
char *timezone_file = "timezonelist";
#else
char *timezone_file = "/usr/share/controlpanel/resource/timezonelist";
#endif
static char *confirmLabel = N_("This change will take effect after a restart.");

/*--------------------------------------------------------------------
 * Global Data
 *------------------------------------------------------------------*/
char TZData[512][256];
gint TZCnt;
char currentTZ[256] = "";
char newTZ[256] = "";

/*-------------------------------------------------------------------
 * Function   : combo_changed
 *------------------------------------------------------------------*/
/**
 * Combobox Changed signal Callback
 * @param     GtkComboBox *widget
 *            gpointer gdata
 *
 * @return    non
 * @note      nothing
 */
/*------------------------------------------------------------------*/
static void combo_changed (GtkComboBox * combo, gpointer gdata)
{
    GtkWidget *treeview;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GtkTreePath *path;
    int i, cnt, idx;
    char *city;

    treeview = (GtkWidget *)gdata;

    /* Obtain currently selected string from combo box */
    gchar *region = gtk_combo_box_get_active_text (combo);

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
    gtk_list_store_clear (GTK_LIST_STORE (model));

    cnt = idx = 0;
    for (i = 0; i < TZCnt; i++) {
        city = strstr (TZData[i], region);
        if (city != NULL) {
            city = strstr (city, "/");
            gtk_list_store_append (GTK_LIST_STORE (model), &iter);
            gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                    COLUMN_ITEM, ++city, -1);
            if (!strcmp (TZData[i], currentTZ)) {
                selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
                gtk_tree_selection_select_iter (selection, &iter);
                idx = cnt;
            }
            cnt++;
        }
    }
    path = gtk_tree_path_new_from_indices(idx, -1);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(treeview), path, NULL, TRUE, 0, 0);

    g_free (region);
}

static void timezone_data_set(void)
{
    ULONG hci_ret;

    if (strcmp(currentTZ, newTZ) != 0) {
#ifndef PCDEBUG
        hci_ret = set_hci_string(VAL_TIME_ZONE, newTZ);	/* 2016/11/07 TOPS Add */
#else
        g_print("selected TimeZone = currentTZ [%s] -> [%s]\n",currentTZ, newTZ);
        hci_ret = HCI_SUCCESS;
#endif
        if (hci_ret == HCI_SUCCESS)
            strcpy(currentTZ, newTZ);
    }
    gtk_main_quit();
}

/*-------------------------------------------------------------------
 * Function   : timezone_set
 *------------------------------------------------------------------*/
/**
 * timezone lit set to ComboBox
 * @param     GtkWidget *combo
 *
 * @return    EXIT Code
 * @note      nothing
 */
/*------------------------------------------------------------------*/
static void timezone_set (GtkWidget * combo, char *tz)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    char count[3];
    int i;
    gchar *name;

    model = gtk_combo_box_get_model (GTK_COMBO_BOX(combo));

    name = g_new0(gchar, 256);
    for (i = 0; i < MAX_REGION; i++) {
        snprintf (count, 3, "%d", i);
        if (gtk_tree_model_get_iter_from_string (model, &iter, count)) {
            gtk_tree_model_get (model, &iter, 0, &name, -1);
            if (!strcmp (tz, name)) {
                break;
            }
        } else {
            gtk_combo_box_append_text (GTK_COMBO_BOX (combo), tz);
            break;
        }
    }
    g_free (name);
}

static GtkWidget *create_combobox()
{
    GtkWidget *combo;
    int i;
    char tz[256];
    combo = gtk_combo_box_new_text ();

    for (i = 0; i< TZCnt; i++) {
        strcpy (tz, TZData[i]);
        strtok (tz, "/");
        timezone_set (combo, tz);
    }

    return combo;
}

static void read_timezone_file()
{

    FILE *fp = NULL;
    char timezone[256];
    char *p;
    int idx = 0;

    /* 2016/11/07 TOPS Mod Start */
    char currentTimeZone[256];
    ULONG hci_ret;

    memset (currentTimeZone, '\0', sizeof (currentTimeZone));
#ifdef PCDEBUG
    strcpy (currentTimeZone, "Asia/Tokyo");
    hci_ret = HCI_SUCCESS;
#else
    hci_ret = get_hci_string (VAL_TIME_ZONE, currentTimeZone);
#endif
    if ((hci_ret == HCI_FAIL) || (strcmp (currentTimeZone, "") == 0))
    {
        /* ERROR */
        strcpy(currentTZ, "Asia/Tokyo");	//UTC?
    }
    else
    {
        strcpy(currentTZ, currentTimeZone);
    }

    fp = fopen (timezone_file, "r");
    if (fp)
    {
        while (fgets (timezone, sizeof (timezone) - 1, fp) != NULL)
        {
            if ((p = strchr (timezone, '\n')) != NULL)
            {
                *p = '\0';
            }
            strcpy(TZData[idx], timezone);
            if (strcmp (timezone, currentTZ) == 0)
            {
                strcpy (newTZ, timezone);
            }
            idx++;
        }
    }
    else
    {
        perror ("fopenTimezoneFile");
    }
    fclose(fp);
    TZCnt = idx;
}

static void cursor_treeview(GtkTreeView *treeview, gpointer user_data)
{
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkWidget *combo;
    gchar *region, *city;

    selection = gtk_tree_view_get_selection(treeview);
    if (!selection)
        return;
    if (!gtk_tree_selection_get_selected(selection, &model, &iter))
        return;

    combo = (GtkWidget *)user_data;
    region = gtk_combo_box_get_active_text (GTK_COMBO_BOX(combo));
    gtk_tree_model_get (model, &iter, COLUMN_ITEM, &city, -1);

    snprintf (newTZ, 256, "%s/%s", region, city);
#ifdef PCDEBUG
    g_print("cursor_treeview: newTZ:%s\n", newTZ);
#endif
    g_free (region);
    g_free (city);
    return;
}

static GtkWidget *create_tree_view()
{
    GtkWidget *treeview;
    GtkListStore *liststore;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    liststore = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING);
    treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));
    gtk_widget_set_name (treeview, "CPTimeZoneTreeView");

    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);
    g_object_unref (liststore);

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

    return treeview;
}

static void combo_set_active_string (GtkWidget *combo, gchar *region)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    char count[3];
    int i;
    gchar *name;

    name = g_new0(gchar, 256);
    model = gtk_combo_box_get_model (GTK_COMBO_BOX(combo));
    for (i = 0; i < MAX_REGION; i++) {
        snprintf (count, 3, "%d", i);
        if (gtk_tree_model_get_iter_from_string (model, &iter, count)) {
            gtk_tree_model_get (model, &iter, 0, &name, -1);
            if (!strcmp (region, name)) {
                gtk_combo_box_set_active(GTK_COMBO_BOX(combo), i);
                break;
            }
        }
    }
    g_free (name);
    return;
}

static void set_data_widget(GtkWidget *combo, GtkWidget *treeview)
{
    gchar *region;
    gchar *city;
    char *p;

    region = g_new0(gchar, 256);
    city = g_new0(gchar, 256);

    strcpy (region, currentTZ);
    strtok (region, "/");
    p = strstr (currentTZ, "/");
    strcpy (city, ++p);

    if (city != NULL) {
#ifdef PCDEBUG
        g_print("region:%s city:%s\n", region, city);
#endif
        combo_set_active_string(combo, region);
    }

    g_free(region);
    g_free(city);
}

static GtkWidget *create_timezone_widget()
{
    GtkWidget *widget, *combo, *treeview, *scroll;
    /* GtkCellRenderer *renderer; */
    widget = gtk_vbox_new(FALSE, 0);

    /* read timezone file */
    read_timezone_file();

    /* Create combo box */
    combo = create_combobox ();
    gtk_widget_modify_font (GTK_WIDGET (gtk_bin_get_child((GtkBin *)combo)),
            pango_font_description_from_string("Sans 15"));
    gtk_widget_set_name (combo, "CPTimeZoneComboBox");

    /* Create Treeview */
    treeview = create_tree_view();
    scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scroll), treeview);

    /* Connect signal to tour handler function */
    g_signal_connect (GTK_OBJECT (treeview), "cursor-changed",
            G_CALLBACK (cursor_treeview), (gpointer)combo);
    g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK (combo_changed),
            (gpointer)treeview);

    /* set data in treeview and active */
    set_data_widget(combo, treeview);

    gtk_box_pack_start (GTK_BOX (widget), combo, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (widget), scroll, TRUE, TRUE, 10);

    return widget;
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
GtkWidget *TzcsCPTimeZone(void)
{
    GtkWidget *vbox;
    GtkWidget *frame;
    GtkWidget *tzwidget, *table;
    //Add 09/08
    GtkWidget *label, *tlabel, *flabel, *button;
    int lwidth, lheight, xbase, ybase;

    gtk_rc_parse_string (
            "widget \"*.CPTimeZoneTreeView\" style \"TZCS2\"\n"
            "widget \"*.CPTimeZoneComboBox.*\" style \"TZCS3\"\n");

    /* Create PackingBox */
    vbox = gtk_vbox_new (FALSE, WIN_BORDER);

    table = gtk_table_new (568, 708, TRUE);

    xbase = 24;
    ybase = 20;
    tlabel = gtk_label_new (_("TimeZone"));
    label_modify_depend_lang (tlabel, "Sans", 34, &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(table), tlabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
    ybase = ybase + lheight + 24;

    //Add 09/08
    label = gtk_label_new (_(confirmLabel));
    label_modify_depend_lang (label, "Sans", 18, &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(table), label,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);
    ybase = ybase + lheight + 24;

    /* Create frame */
    frame = gtk_frame_new (_("TimeZone :"));
    flabel =  gtk_frame_get_label_widget(GTK_FRAME(frame));
    gtk_widget_modify_font (GTK_WIDGET(flabel),
            pango_font_description_from_string("Sans 14"));

    tzwidget = create_timezone_widget();

    gtk_container_add (GTK_CONTAINER (frame), tzwidget);
    gtk_table_attach(GTK_TABLE(table), frame,
            xbase, xbase + 400,
            ybase, ybase + 300,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    button = gtk_button_new_with_label(_("OK"));
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
            GTK_SIGNAL_FUNC(timezone_data_set), NULL);
    label_color_white(gtk_bin_get_child((GtkBin *)button));
    widget_color_blue(button);
    ybase = ybase + 315;
    xbase = xbase + 430;
    gtk_table_attach(GTK_TABLE(table), button,
            xbase, xbase + 100,
            ybase, ybase + 32,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    /* packing */
    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);

    return vbox;
}
