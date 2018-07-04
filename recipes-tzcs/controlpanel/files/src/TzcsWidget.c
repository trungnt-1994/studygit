#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

/* /usr/share/X11/rgb.txt */
GdkColor white = {0, 65535, 65535, 65535};
GdkColor TzcsBlue = {0, 12336, 30326, 48316};
/* GdkColor TzcsBlue = {0, 3932, 29490, 47840}; */

void Widget_color_white(GtkWidget *widget)
{
    gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, &white);
    gtk_widget_modify_bg(widget, GTK_STATE_ACTIVE, &white);
    gtk_widget_modify_bg(widget, GTK_STATE_PRELIGHT, &white);
    gtk_widget_modify_bg(widget, GTK_STATE_SELECTED, &white);
}

void widget_color_blue(GtkWidget *widget)
{
    /* gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE); */
    gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, &TzcsBlue);
    gtk_widget_modify_bg(widget, GTK_STATE_ACTIVE, &TzcsBlue);
    gtk_widget_modify_bg(widget, GTK_STATE_PRELIGHT, &TzcsBlue);
    gtk_widget_modify_bg(widget, GTK_STATE_SELECTED, &TzcsBlue);

}

void label_color_white(GtkWidget *widget)
{
    gtk_widget_modify_fg(widget, GTK_STATE_NORMAL, &white);
    gtk_widget_modify_fg(widget, GTK_STATE_ACTIVE, &white);
    gtk_widget_modify_fg(widget, GTK_STATE_PRELIGHT, &white);
    gtk_widget_modify_bg(widget, GTK_STATE_SELECTED, &white);
}

void get_pixel_from_label(GtkWidget *label, const gchar *font,
        gint *width, gint *height)
{
    PangoLayout *layout;
    PangoFontDescription *desc;

    layout = gtk_widget_create_pango_layout(
            label,
            gtk_label_get_label(GTK_LABEL(label)));
    desc = pango_font_description_from_string(font);
    pango_layout_set_font_description(layout, desc);
    pango_layout_get_pixel_size(layout, width, height);

    pango_font_description_free(desc);
    g_object_unref(layout);

}

void label_modify_depend_lang (GtkWidget *label, const char *font, int size,
        int *lwidth, int *lheight)
{
    int weight;
    char *lang, *font_set;

    lang = getenv ("LANG");
    if (!strcmp (lang, "ja_JP.UTF-8"))
        if (size == 34)
            weight = -5;
        else
            weight = -2;
    else
        weight = 0;

    font_set = g_new0 (char, 32);
    sprintf(font_set,"%s %d", font, size + weight);
    gtk_widget_modify_font(GTK_WIDGET(label),
            pango_font_description_from_string(font_set));
    get_pixel_from_label(label, (const gchar *)font_set,
            lwidth, lheight);
    g_free (font_set);
}
