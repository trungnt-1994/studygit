#include <gtk/gtk.h>
#include <stdlib.h>
#include "tzcsI18N.h"
#include <locale.h>

#define WIN_BORDER 10

void label_color_white(GtkWidget *widget);
void widget_color_blue(GtkWidget *widget);
void label_modify_depend_lang (GtkWidget *label, const char *font, int size,
        int *lwidth, int *lheight);

static void eula_onclicked(GtkWidget *widget, gpointer data)
{
#ifdef PCDEBUG
    system("./ExecEula &");
#else
    system("ExecEula &");
#endif
    gtk_main_quit ();
}

GtkWidget *TzcsEULA(void)
{
    GtkWidget *widget;
    GtkWidget *elabel, *desclabel, *eula_button, *opensrc_btn, *table;
    GtkWidget *eula_hbox;
    GtkWidget *eula_label;
    GtkWidget *opensrc_hbox;
    GtkWidget *opensrc_label;
    int lwidth, lheight, xbase, ybase;

    widget = gtk_vbox_new(FALSE, WIN_BORDER);
    table = gtk_table_new(568, 708, TRUE);
    gtk_box_pack_start(GTK_BOX(widget), table, FALSE, FALSE, 0);
    xbase = 24;
    ybase = 20;

    elabel = gtk_label_new(_("End User License Agreement"));
    gtk_widget_modify_font (GTK_WIDGET(elabel), pango_font_description_from_string("Sans 34"));
    get_pixel_from_label(elabel, (const gchar *)"Sans 34", &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(table), elabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    xbase = xbase + 30;
    ybase = ybase + lheight + 15;
    desclabel = gtk_label_new(_("You can refer to the end user license agreement and the open\n\
                source license notice with relation to this system."));
    label_modify_depend_lang (desclabel, "Sans", 14, &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(table), desclabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    ybase = ybase + lheight + 50;
    eula_label = gtk_label_new(_("End user license agreement:"));
    label_modify_depend_lang (eula_label, "Sans", 14, &lwidth, &lheight);
    eula_button = gtk_button_new_with_label(_("Open"));
    gtk_widget_set_size_request(eula_button, 70, 20);
    label_color_white(gtk_bin_get_child((GtkBin *)eula_button));
    widget_color_blue(eula_button);
    gtk_signal_connect(GTK_OBJECT(eula_button), "clicked", GTK_SIGNAL_FUNC(eula_onclicked), NULL);

    eula_hbox = gtk_hbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(eula_hbox), eula_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(eula_hbox), eula_button, FALSE, FALSE, 0);

    gtk_table_attach(GTK_TABLE(table), eula_hbox,
            xbase, 568,
            ybase, ybase + 32,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    ybase = ybase + 32 + 50;
    opensrc_label = gtk_label_new(_("Open source license notice:"));
    label_modify_depend_lang (opensrc_label, "Sans", 14, &lwidth, &lheight);
    opensrc_btn = gtk_button_new_with_label(_("Open"));
    gtk_widget_set_size_request(opensrc_btn, 70, 20);
    label_color_white(gtk_bin_get_child((GtkBin *)opensrc_btn));
    widget_color_blue(opensrc_btn);
    gtk_signal_connect(GTK_OBJECT(opensrc_btn), "clicked", GTK_SIGNAL_FUNC(eula_onclicked), NULL);

    opensrc_hbox = gtk_hbox_new(FALSE, 20);
    gtk_box_pack_start(GTK_BOX(opensrc_hbox), opensrc_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(opensrc_hbox), opensrc_btn, FALSE, FALSE, 0);

    gtk_table_attach(GTK_TABLE(table), opensrc_hbox,
            xbase, 568,
            ybase, ybase + 32,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    return widget;
}
