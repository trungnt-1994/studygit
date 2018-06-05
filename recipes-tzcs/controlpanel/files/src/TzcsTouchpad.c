/************************************************
 * TzcsTouchpad.c
 *
 * Copyright 2016-2017 TOSHIBA DEVELOPMENT & ENGINEERINGS CO.,LTD.
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2017/06/21  DME
 *  - New
 *       2017/08/22  DME
 *  - TouchPad_Settings.inf update
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "tzcsI18N.h"
#include <locale.h>

/*--------------------------------------------------------------------
 * Global Definitions
 *------------------------------------------------------------------*/
#define WIN_BORDER          10

#if 0
#define TP_DEBUG			1
#else
#undef  TP_DEBUG
#endif

/* TouchPad Settings Structure Version */
#define	MAJOR_VERSION		0x01
#define	MINOR_VERSION		0x02

void get_pixel_from_label(GtkWidget *label, const gchar *font,
        gint *width, gint *height);

/*--------------------------------------------------------------------
 * Resouces
 *------------------------------------------------------------------*/
typedef struct {
    gchar		gMajor_version;
    gchar		gMinor_version;
    gint		gTouchPad_onoff;
    double		gWait_Time;
    double		gPoll_Delay;
    gboolean	gPalmRejection_Active;
    gboolean	Malfunction_function;
} TouchPad_Settings;

char *touchpad_file = "/var/tmp/TouchPad_Settings.inf";

/*--------------------------------------------------------------------
 * Global Data
 *------------------------------------------------------------------*/
GtkWidget	*window;
GtkWidget	*scale_frame;
GtkWidget	*scale_flabel;
GtkWidget	*wait_label;
GtkWidget	*wait_scale;
GtkWidget	*delay_label;
GtkWidget	*delay_scale;

gdouble		wait_min = 500 , wait_max = 2000 , wait_step = 500;
gdouble		delay_min = 50 , delay_max = 300 , delay_step = 50;

gchar		gMajor_version;
gchar		gMinor_version;
gint		gTouchPad_onoff;
gboolean	gPalmRejection_Active;
gboolean	Malfunction_function;
double		gWait_Time;			// Forbidden period after keyboard input.(milli-seconds)
double		gPoll_Delay;		// Wait until next poll.(milli-seconds)

TouchPad_Settings	gTouchPad_Settings;

int		gLANG_JP = FALSE;

/*-------------------------------------------------------------------
 * Function   : Check Box Button Call Back (Palm Rejection Enable)
 *------------------------------------------------------------------*/
static void cb_button_toggled (GtkToggleButton *widget , gpointer data) {
    gboolean	active;
#ifdef TP_DEBUG
    gchar		*str[] = {"FALSE" , "TRUE"};
#endif

    active = gtk_toggle_button_get_active (widget);
    gPalmRejection_Active = active;
#ifdef TP_DEBUG
    g_print ("Palm Rejection Check button state : %s\n" , str[active]);
#endif
}

/*----------------------------------------------------------------------------------
 * Function   : Check Box Button Call Back (Malfunction inhibit function is enabled)
 *----------------------------------------------------------------------------------*/
static void cb_button_toggled_1 (GtkToggleButton *widget , gpointer data) {
    gboolean	active;
#ifdef TP_DEBUG
    gchar		*str[] = {"FALSE" , "TRUE"};
#endif
    GdkColor	color;

    active = gtk_toggle_button_get_active (widget);
    Malfunction_function = active;
#ifdef TP_DEBUG
    g_print ("Malfunction function Check button state : %s\n" , str[active]);
#endif
    if ( Malfunction_function == 1 ) {
        gdk_color_parse ("black", &color);
    } else {
        gdk_color_parse ("gray", &color);
    }

    gtk_widget_modify_fg (scale_frame,  GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg (scale_flabel, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg (wait_label,   GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg (wait_scale,   GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg (delay_label,  GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg (delay_scale,  GTK_STATE_NORMAL, &color);
}

/*-------------------------------------------------------------------
 * Function   : Scale Changed Call Back (Wait Time)
 *------------------------------------------------------------------*/
static void cb_wait_value_changed (GtkScale *scale , gpointer data) {
    double		Tempt_Time;
    gint		step_cnt;
    gint		step_remainder;

    if ( Malfunction_function == 1 )
    {
        Tempt_Time = gtk_range_get_value (GTK_RANGE(scale));
        step_cnt = (gint)(Tempt_Time / wait_step);
        step_remainder = (gint)(Tempt_Time - (step_cnt * wait_step));

        if( step_remainder < ( (gint)wait_step / 2) ) {
            gWait_Time = (step_cnt * wait_step);
            if( gWait_Time < wait_min)
                gWait_Time = wait_min;
        } else {
            if( ((step_cnt * wait_step) + wait_step) < wait_max) {
                gWait_Time = (step_cnt * wait_step) + wait_step;
            } else {
                gWait_Time = wait_max;
            }
        }
#ifdef TP_DEBUG
        g_print ("cb_wait_value_changed Cnt = %d (%d)\n" , step_cnt, step_remainder);
        g_print ("cb_wait_value_changed Time = %f (%f)\n" , Tempt_Time, gWait_Time);
#endif
        gtk_range_set_value (GTK_RANGE(scale) , gWait_Time);
    } else {
        gtk_range_set_value (GTK_RANGE(scale) , gWait_Time);
    }
}

/*-------------------------------------------------------------------
 * Function   : Scale Changed Call Back (Delay Time)
 *------------------------------------------------------------------*/
static void cb_delay_value_changed (GtkScale *scale , gpointer data) {
    double		Tempt_Time;
    gint		step_cnt;
    gint		step_remainder;

    if ( Malfunction_function == 1 )
    {
        Tempt_Time = gtk_range_get_value (GTK_RANGE(scale));
        step_cnt = (gint)(Tempt_Time / delay_step);
        step_remainder = (gint)(Tempt_Time - (step_cnt * delay_step));

        if( step_remainder < ( (gint)delay_step / 2) ) {
            gPoll_Delay = (step_cnt * delay_step);
            if( gPoll_Delay < delay_min)
                gPoll_Delay = delay_min;
        } else {
            if( ((step_cnt * delay_step) + delay_step) < delay_max) {
                gPoll_Delay = (step_cnt * delay_step) + delay_step;
            } else {
                gPoll_Delay = delay_max;
            }
        }
#ifdef TP_DEBUG
        g_print ("cb_delay_value_changed Cnt = %d (%d)\n" , step_cnt, step_remainder);
        g_print ("cb_delay_value_changed Time = %f (%f)\n" , Tempt_Time, gPoll_Delay);
#endif
        gtk_range_set_value (GTK_RANGE(scale) , gPoll_Delay);
    } else {
        gtk_range_set_value (GTK_RANGE(scale) , gPoll_Delay);
    }
}


/*-------------------------------------------------------------------
 * Function   : Apply Button Call Back
 *------------------------------------------------------------------*/
void buttonOK_clicked(GtkWidget * widget, gpointer data)
{
#ifdef TP_DEBUG
    g_print ("BUTTON_Apply : %s was selected\n", (char *) data);
#endif

    int fd_touchpad;

    fd_touchpad = open(touchpad_file, O_SYNC | O_RDWR);

    if (fd_touchpad < 0) {
#ifdef TP_DEBUG
        g_print ("buttonOK_clicked : File Open Error (settings file)\n");
#endif
    }else{
        gTouchPad_Settings.gPalmRejection_Active = gPalmRejection_Active;
        gTouchPad_Settings.Malfunction_function = Malfunction_function;
        gTouchPad_Settings.gWait_Time = gWait_Time;
        gTouchPad_Settings.gPoll_Delay = gPoll_Delay;

        lseek(fd_touchpad, 0, SEEK_SET);
        write(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));
        close(fd_touchpad);
    }

    system("killall syndaemon_tos > /dev/null 2>&1");

    if( Malfunction_function == 1 )
    {
        system("syndaemon_start > /dev/null 2>&1");
    }

    /* Copy to a current folder from save folder. */
    system("cp /var/tmp/TouchPad_Settings.inf TouchPad_Settings.inf  > /dev/null 2>&1");

    /* TouchPad settings write to BIOS */
    system("tenco fvw 3 TouchPad_Settings.inf  > /dev/null 2>&1");

    gtk_main_quit ();
}

/*-------------------------------------------------------------------
 * Function   : initial settings
 *------------------------------------------------------------------*/
void initial_setting()
{
    int				fd_touchpad;
    struct stat		st;

    if(stat(touchpad_file, &st) != 0){
#ifdef TP_DEBUG
        g_print ("initial_setting : Since there is no settings file, it reads from BIOS.\n");
#endif
        /* TouchPad settings read from BIOS */
        system("tenco fvr 3 TouchPad_Settings.inf > /dev/null 2>&1");

        /* Copy to a save folder from current folder. */
        system("cp TouchPad_Settings.inf /var/tmp/TouchPad_Settings.inf > /dev/null 2>&1");
    }

    fd_touchpad = open(touchpad_file, O_SYNC | O_RDWR);

    if (fd_touchpad < 0) {
#ifdef TP_DEBUG
        g_print ("initial_setting : File Open Error (set to default settings)\n");
#endif
        gMajor_version = MAJOR_VERSION;
        gMinor_version = MINOR_VERSION;
        gTouchPad_onoff = 1;
        gPalmRejection_Active = 1;
        Malfunction_function = 1;
        gWait_Time = 1000;
        gPoll_Delay = 200;

        gTouchPad_Settings.gMajor_version = MAJOR_VERSION;
        gTouchPad_Settings.gMinor_version = MINOR_VERSION;
        gTouchPad_Settings.gTouchPad_onoff = 1;
        gTouchPad_Settings.gPalmRejection_Active = gPalmRejection_Active;
        gTouchPad_Settings.Malfunction_function = Malfunction_function;
        gTouchPad_Settings.gWait_Time = gWait_Time;
        gTouchPad_Settings.gPoll_Delay = gPoll_Delay;

        fd_touchpad = open(touchpad_file, O_SYNC | O_CREAT | O_RDWR, 0x666);
        lseek(fd_touchpad, 0, SEEK_SET);
        write(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));
        close(fd_touchpad);
    }else{
        read(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));

        if( gTouchPad_Settings.gMajor_version != MAJOR_VERSION
                ||  gTouchPad_Settings.gMinor_version != MINOR_VERSION ) {
#ifdef TP_DEBUG
            g_print ("initial_setting : TouchPad Settings Structure Version Discord (Set with default)\n");
#endif
            /* TouchPad Settings Structure Version Discord (Set with default) */
            gMajor_version = MAJOR_VERSION;
            gMinor_version = MINOR_VERSION;
            gTouchPad_onoff = 1;
            gPalmRejection_Active = 1;
            Malfunction_function = 1;
            gWait_Time = 1000;
            gPoll_Delay = 200;

            gTouchPad_Settings.gMajor_version = MAJOR_VERSION;
            gTouchPad_Settings.gMinor_version = MINOR_VERSION;
            gTouchPad_Settings.gTouchPad_onoff = 1;
            gTouchPad_Settings.gPalmRejection_Active = gPalmRejection_Active;
            gTouchPad_Settings.Malfunction_function = Malfunction_function;
            gTouchPad_Settings.gWait_Time = gWait_Time;
            gTouchPad_Settings.gPoll_Delay = gPoll_Delay;

            lseek(fd_touchpad, 0, SEEK_SET);
            write(fd_touchpad, &gTouchPad_Settings, sizeof(gTouchPad_Settings));
        }else{
#ifdef TP_DEBUG
            g_print ("initial_setting : TouchPad Settings Structure Version is OK\n");
#endif
            gMajor_version = gTouchPad_Settings.gMajor_version;
            gMinor_version = gTouchPad_Settings.gMinor_version;
            gTouchPad_onoff = gTouchPad_Settings.gTouchPad_onoff;
            gPalmRejection_Active = gTouchPad_Settings.gPalmRejection_Active;
            Malfunction_function = gTouchPad_Settings.Malfunction_function;
            gWait_Time = gTouchPad_Settings.gWait_Time;
            gPoll_Delay = gTouchPad_Settings.gPoll_Delay;
        }

        close(fd_touchpad);
    }

    if( memcmp("ja_JP.UTF-8", getenv("LANG"), sizeof("ja_JP.UTF-8")) == 0 )
        gLANG_JP = TRUE;
    else
        gLANG_JP = FALSE;
}


/*-------------------------------------------------------------------
 * Function   : main
 *------------------------------------------------------------------*/
/**
 * TZCS Control Panel Touchpad
 * @param     nothing
 *
 * @return    EXIT Code
 * @note      nothing
 */
/*------------------------------------------------------------------*/
GtkWidget *TzcsTouchpad(void)
{
    GtkWidget	*main_table;
    GtkWidget	*vbox;
    GtkWidget	*frame_vbox;
    GtkWidget	*table_frame_vbox;
    GtkWidget	*chk_box;
    GtkWidget	*box_button;
    GtkWidget	*box_button_1;
    GtkWidget	*buttonOK;
    GdkColor	color;
    GtkWidget	*tlabel;
    GtkWidget	*dlabel;
    GtkWidget	*chk_hbox;
    GtkWidget	*chk_hbox_label;
    GtkWidget	*chk_hbox_1;
    GtkWidget	*chk_hbox_1_label;
    GtkWidget	*wait_hbox;
    GtkWidget	*wait_slabel;
    GtkWidget	*wait_llabel;
    GtkWidget	*delay_hbox;
    GtkWidget	*delay_slabel;
    GtkWidget	*delay_llabel;
    GdkColor	white_color;
    int			lwidth, lheight, xbase, ybase;


    initial_setting();

    /***************************************************/
    /* Create PackingBox & Table (TouchPad Top Window) */
    /***************************************************/
    vbox = gtk_vbox_new (FALSE, WIN_BORDER);
    main_table = gtk_table_new(568, 708, TRUE);
    gtk_box_pack_start (GTK_BOX (vbox), main_table, FALSE, FALSE, 0);
    xbase = 24;
    ybase = 20;

    /*******************************************/
    /* Create label (TouchPad Title Label)     */
    /*******************************************/
    tlabel = gtk_label_new(_("Touch Pad"));

    if( gLANG_JP == TRUE ) {
        gtk_widget_modify_font (GTK_WIDGET(tlabel),
                pango_font_description_from_string("Sans 29"));
        get_pixel_from_label(tlabel, (const gchar *)"Sans 29", &lwidth, &lheight);
    } else {
        gtk_widget_modify_font (GTK_WIDGET(tlabel),
                pango_font_description_from_string("Sans 34"));
        get_pixel_from_label(tlabel, (const gchar *)"Sans 34", &lwidth, &lheight);
    }
#ifdef TP_DEBUG
    g_print ("tlabel size width:%d height:%d \n" , lwidth, lheight);
#endif

    gtk_table_attach(GTK_TABLE(main_table), tlabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    if( gLANG_JP == TRUE )
        ybase = ybase + lheight + 20;
    else
        ybase = ybase + lheight + 24;

    /***************************************************/
    /* Create lable (Configure Touch Pad settings)     */
    /***************************************************/
    dlabel = gtk_label_new(_("Configure Touch Pad settings."));
    if( gLANG_JP == TRUE ) {
        gtk_widget_modify_font (GTK_WIDGET(dlabel),
                pango_font_description_from_string("Sans 16"));
        get_pixel_from_label(dlabel, (const gchar *)"Sans 16", &lwidth, &lheight);
    } else {
        gtk_widget_modify_font (GTK_WIDGET(dlabel),
                pango_font_description_from_string("Sans 18"));
        get_pixel_from_label(dlabel, (const gchar *)"Sans 18", &lwidth, &lheight);
    }
    gtk_table_attach(GTK_TABLE(main_table), dlabel,
            xbase, xbase + lwidth,
            ybase, ybase + lheight,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    if( gLANG_JP == TRUE ) {
        ybase = ybase + lheight + 18;
        xbase = xbase + 45;
    } else {
        ybase = ybase + lheight + 22;
        xbase = xbase + 50;
    }

    /*******************************************/
    /* Create frame (TouchPad Top Frame)       */
    /*******************************************/
    frame_vbox = gtk_vbox_new (FALSE, 0);
    gtk_table_attach(GTK_TABLE(main_table), frame_vbox,
            xbase , 568,
            ybase , 708,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 0);

    /*******************************************/
    /* Create CheckBox (Palm Rejection Enable) */
    /*******************************************/
    chk_box = gtk_vbox_new (FALSE , 0);
    gtk_box_pack_start (GTK_BOX(frame_vbox), chk_box, FALSE, FALSE, 0);

    chk_hbox = gtk_hbox_new(FALSE , 10);
    gtk_box_pack_start (GTK_BOX(chk_box), chk_hbox, FALSE, FALSE, 10);
    box_button = gtk_check_button_new ();
    if( gLANG_JP == TRUE ) {
        gtk_widget_modify_font(GTK_WIDGET(box_button),
                pango_font_description_from_string("Sans 12"));
    } else {
        gtk_widget_modify_font(GTK_WIDGET(box_button),
                pango_font_description_from_string("Sans 14"));
    }
    gtk_box_pack_start (GTK_BOX(chk_hbox) , box_button ,   FALSE , FALSE , 0);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(box_button) , gPalmRejection_Active);
    g_signal_connect (G_OBJECT(box_button) , "toggled" , G_CALLBACK (cb_button_toggled) , NULL);
    chk_hbox_label = gtk_label_new (_("Enable Palm Rejection"));
    if( gLANG_JP == TRUE ) {
        gtk_widget_modify_font(GTK_WIDGET(chk_hbox_label),
                pango_font_description_from_string("Sans 12"));
    } else {
        gtk_widget_modify_font(GTK_WIDGET(chk_hbox_label),
                pango_font_description_from_string("Sans 14"));
    }
    gtk_box_pack_start (GTK_BOX(chk_hbox) , chk_hbox_label , FALSE , FALSE , 0);
    gtk_misc_set_alignment(GTK_MISC(chk_hbox_label), 0, 0);

    chk_hbox_1 = gtk_hbox_new(FALSE , 10);
    gtk_box_pack_start (GTK_BOX(chk_box), chk_hbox_1, FALSE, FALSE, 5);
    box_button_1 = gtk_check_button_new ();
    if( gLANG_JP == TRUE ) {
        gtk_widget_modify_font(GTK_WIDGET(box_button_1),
                pango_font_description_from_string("Sans 12"));
    } else {
        gtk_widget_modify_font(GTK_WIDGET(box_button_1),
                pango_font_description_from_string("Sans 14"));
    }
    gtk_box_pack_start (GTK_BOX(chk_hbox_1) , box_button_1 ,   FALSE , FALSE , 0);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(box_button_1) , Malfunction_function);
    g_signal_connect (G_OBJECT(box_button_1) , "toggled" , G_CALLBACK (cb_button_toggled_1) , NULL);
    chk_hbox_1_label = gtk_label_new (_("Disable Touch Pad during key input"));
    if( gLANG_JP == TRUE ) {
        gtk_widget_modify_font(GTK_WIDGET(chk_hbox_1_label),
                pango_font_description_from_string("Sans 12"));
    } else {
        gtk_widget_modify_font(GTK_WIDGET(chk_hbox_1_label),
                pango_font_description_from_string("Sans 14"));
    }
    gtk_box_pack_start (GTK_BOX(chk_hbox_1) , chk_hbox_1_label , FALSE , FALSE , 0);
    gtk_misc_set_alignment(GTK_MISC(chk_hbox_1_label), 0, 0);


    /*****************************************/
    /* Create Scale (Wait Time & Delay Time) */
    /*****************************************/
    table_frame_vbox = gtk_table_new(35, 60, FALSE);
    gtk_box_pack_start(GTK_BOX(frame_vbox), table_frame_vbox, FALSE, FALSE, 35);

    wait_hbox = gtk_hbox_new(FALSE, 10);
    wait_slabel = gtk_label_new(_("[Short]"));
    wait_llabel = gtk_label_new(_("[Long]"));
    wait_scale = gtk_hscale_new_with_range(wait_min, wait_max, wait_step);
    gtk_range_set_value (GTK_RANGE(wait_scale), gWait_Time);
    gtk_scale_set_draw_value (GTK_SCALE(wait_scale), FALSE);
    g_signal_connect (G_OBJECT(wait_scale) , "value_changed" , G_CALLBACK(cb_wait_value_changed) , NULL);
    gtk_box_pack_start(GTK_BOX(wait_hbox), wait_slabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(wait_hbox), wait_scale, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(wait_hbox), wait_llabel, FALSE, FALSE, 0);
    gtk_table_attach(GTK_TABLE(table_frame_vbox), wait_hbox,
            1, 60,
            1, 4,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 50, 0);
    wait_label = gtk_label_new(_("Interval when Touch Pad operation is disabled"));
    gtk_table_attach(GTK_TABLE(table_frame_vbox), wait_label,
            1, 60,
            5, 9,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 90, 5);

    GtkWidget *space = gtk_label_new("");
    gtk_table_attach(GTK_TABLE(table_frame_vbox), space,
            1, 60,
            10, 14,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 90, 5);

    delay_hbox = gtk_hbox_new(FALSE, 10);
    delay_slabel = gtk_label_new(_("[Short]"));
    delay_llabel = gtk_label_new(_("[Long]"));
    delay_scale = gtk_hscale_new_with_range(delay_min, delay_max, delay_step);
    gtk_range_set_value(GTK_RANGE(delay_scale), gPoll_Delay);
    gtk_scale_set_draw_value(GTK_SCALE(delay_scale), FALSE);
    g_signal_connect(G_OBJECT(delay_scale), "value_changed", G_CALLBACK(cb_delay_value_changed), NULL);
    gtk_box_pack_start(GTK_BOX(delay_hbox), delay_slabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(delay_hbox), delay_scale, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(delay_hbox), delay_llabel, FALSE, FALSE, 0);
    gtk_table_attach(GTK_TABLE(table_frame_vbox), delay_hbox,
            1, 60,
            15, 19,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 50, 0);
    delay_label = gtk_label_new(_("Waiting interval to next poll"));
    gtk_table_attach(GTK_TABLE(table_frame_vbox), delay_label,
            1, 60,
            20, 24,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND,
            GTK_FILL | GTK_SHRINK | GTK_EXPAND, 130, 5);

    buttonOK = gtk_button_new_with_label (_("OK"));
    gtk_signal_connect (GTK_OBJECT (buttonOK), "clicked", GTK_SIGNAL_FUNC (buttonOK_clicked), (gpointer) "OK");
    label_color_white(gtk_bin_get_child((GtkBin *)buttonOK));
    widget_color_blue(buttonOK);
    gtk_widget_set_size_request(buttonOK, 50, 30);
    gtk_table_attach (GTK_TABLE(table_frame_vbox), buttonOK,  55, 60, 25, 35, GTK_FILL | GTK_SHRINK | GTK_EXPAND, GTK_FILL | GTK_SHRINK | GTK_EXPAND, 0, 15);

    /*****************************/
    /* Scale frame color setting */
    /*****************************/
    if ( Malfunction_function == 1 ) {
        gdk_color_parse ("black", &color);
    } else {
        gdk_color_parse ("gray", &color);
    }

    gdk_color_parse("white", &white_color);
    gtk_widget_modify_fg(scale_frame, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg(scale_flabel, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg(wait_label, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg(wait_scale, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg(delay_label, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_fg(delay_scale, GTK_STATE_NORMAL, &color);

    return vbox;
}
