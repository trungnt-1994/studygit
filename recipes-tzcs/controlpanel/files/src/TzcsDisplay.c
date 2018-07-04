#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <locale.h>
#include <xfce-randr.h>

#include "tzcsI18N.h"

typedef struct {
    guint width;
    guint height;
    gdouble rate;
} DisplayMode;

typedef struct {
    char name[256];
    guint width;
    guint height;
    gdouble rate;
    Rotation rotation;
} DisplayData;

#define ROTATIONS_MASK (RR_Rotate_0 | RR_Rotate_90 | RR_Rotate_180 | RR_Rotate_270)
#define REFLECTIONS_MASK (RR_Reflect_X | RR_Reflect_Y)

void label_color_white(GtkWidget *widget);
void widget_color_blue(GtkWidget *widget);
void label_modify_depend_lang (GtkWidget *label, const char *font, int size,
        int *lwidth, int *lheight);

extern XfceRandr *xfce_randr;

static DisplayMode *mode_list(XRRScreenResources *rsc,
        XRROutputInfo *info)
{
    DisplayMode *modes;
    int i, j;

    modes = g_new0(DisplayMode, info->nmode);
    for (i = 0; i < info->nmode; i++) {
        for (j = 0; j < rsc->nmode; j++) {
            if (info->modes[i] == rsc->modes[j].id) {
                modes[i].width = rsc->modes[j].width;
                modes[i].height = rsc->modes[j].height;
                modes[i].rate =
                    (gdouble)rsc->modes[j].dotClock /
                    ((gdouble)rsc->modes[j].hTotal *
                     (gdouble)rsc->modes[j].vTotal);
            }
        }
    }
    return modes;
}

static DisplayData *display_data(guint *cnt)
{
    Display *disp;
    XRRScreenResources *rsc;
    DisplayMode *modes;
    DisplayData *ddata;
    XRROutputInfo *info;
    XRRCrtcInfo *crtc_info;
    guint cid;
    int i, j;

    disp = XOpenDisplay(NULL);
    rsc = XRRGetScreenResources(disp, DefaultRootWindow(disp));

    cid = 0;
    ddata = g_new0(DisplayData, rsc->noutput);
    for (i = 0; i < rsc->noutput; i++) {
        info = XRRGetOutputInfo(disp, rsc, rsc->outputs[i]);
        if (info->connection != RR_Connected) {
            XRRFreeOutputInfo(info);
            continue;
        }
        strcpy(ddata[cid].name, info->name);
        modes = mode_list(rsc, info);

        if (info->crtc != None) {
            crtc_info = XRRGetCrtcInfo(disp, rsc, info->crtc);
            ddata[cid].rotation = crtc_info->rotation;
            ddata[cid].width = crtc_info->width;
            ddata[cid].height = crtc_info->height;
            for (j = 0; j < info->nmode; j++) {
                if (ddata[cid].rotation & RR_Rotate_90 ||
                        (ddata[cid].rotation & RR_Rotate_270 &&
                         ddata[cid].width == modes[j].height &&
                         ddata[cid].height == modes[j].width)) {
                    ddata[cid].rate = modes[j].rate;
                    break;

                } else if (ddata[cid].width == modes[j].width &&
                        ddata[cid].height == modes[j].height) {
                    ddata[cid].rate = modes[j].rate;
                    break;
                }
            }
            XRRFreeCrtcInfo(crtc_info);
        }

        cid++;
        g_free(modes);
        XRRFreeOutputInfo(info);
    }
    XRRFreeScreenResources(rsc);
    XCloseDisplay(disp);

    *cnt = cid;
    return ddata;
}

static gchar *rotation_name(Rotation rotation)
{
    gchar *name;
    switch (rotation & ROTATIONS_MASK) {
        case RR_Rotate_90: name = _("Left"); break;
        case RR_Rotate_180: name = _("Inverted"); break;
        case RR_Rotate_270: name = _("Right"); break;
        default: name= _("None"); break;
    }
    return name;
}

static gchar *reflection_name(Rotation reflection)
{
    gchar *name;
    switch (reflection & REFLECTIONS_MASK) {
        case RR_Reflect_X: name = _("Horizontal"); break;
        case RR_Reflect_Y: name = _("Vertical"); break;
        case RR_Reflect_X | RR_Reflect_Y:
                           name = _("Horizontal and Vertical"); break;
        default: name = _("None"); break;
    }
    return name;
}

static void button_configure(GtkWidget *widget, gpointer data)
{
#ifdef PCDEBUG
    system("./ExecDisplay &");
#else
    system("ExecDisplay &");
#endif
    gtk_main_quit();
}

GtkWidget *TzcsDisplay(void)
{
    GtkWidget *widget, *lhbox, *btable, *button;
    GtkWidget *displaylabel, *desclabel;
    DisplayData *ddata;
    int i;
    int lwidth, lheight;
    int xbase, ybase;
    guint dcnt;
    gchar ddetail[256];
    const gchar *str_resol = N_("Resolution");
    const gchar *str_ref = N_("Refresh rate");
    const gchar *str_rot = N_("Rotation");
    const gchar *str_refrec = N_("Reflection");

    ddata = display_data(&dcnt);

    widget = gtk_vbox_new(FALSE, 0);
    lhbox = gtk_hbox_new(TRUE, 2);

    for (i = 0; i < dcnt; i++) {
        GtkWidget *label;
        g_snprintf(ddetail, sizeof(ddetail),
                "%s\n\n"
                "  %s: %dx%d\n"
                "  %s: %.1fHz\n"
                "  %s: %s\n"
                "  %s: %s",
                xfce_randr->friendly_name[i],
                _(str_resol), ddata[i].width, ddata[i].height,
                _(str_ref), ddata[i].rate,
                _(str_rot), rotation_name(ddata[i].rotation),
                _(str_refrec), reflection_name(ddata[i].rotation));
        label = gtk_label_new(ddetail);
        gtk_widget_modify_font(GTK_WIDGET(label),
                pango_font_description_from_string("Sans 14"));
        gtk_box_pack_start(GTK_BOX(lhbox), label, TRUE, TRUE, 0);
    }

    btable = gtk_table_new(568, 708, TRUE);
    button = gtk_button_new_with_label(_("Configure"));
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
            GTK_SIGNAL_FUNC(button_configure), NULL);
    label_color_white(gtk_bin_get_child((GtkBin *)button));
    widget_color_blue(button);

    xbase = 24;
    ybase = 20;
    displaylabel = gtk_label_new(_("Display"));
    label_modify_depend_lang (displaylabel, "Sans", 34, &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(btable), displaylabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    ybase = ybase + lheight + 10;
    desclabel = gtk_label_new(
            _("Configure screen settings and layout."));
    label_modify_depend_lang (desclabel, "Sans", 18, &lwidth, &lheight);
    gtk_table_attach(GTK_TABLE(btable), desclabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    ybase = ybase + lheight + 32;
    gtk_table_attach(GTK_TABLE(btable), lhbox,
            xbase, xbase + 670,
            ybase, ybase + 250,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    ybase = ybase + 270 + 7;
    gtk_table_attach(GTK_TABLE(btable), button,
            xbase, xbase + 200,
            ybase, ybase + 32,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    gtk_box_pack_start(GTK_BOX(widget), btable, FALSE, FALSE, 0);
    g_free(ddata);
    return widget;
}
