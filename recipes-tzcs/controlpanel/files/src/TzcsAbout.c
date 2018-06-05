/************************************************
 * AboutTZCS.c
 *
 * Copyright 2016 TOSHIBA PLATFORM SOLUTION CORPORATION
 *
 ************************************************/

/***********************************************
 *
 * Change history
 *
 * TZCS  2017/01/16  TSDV
 *  - New
 *  -
 *  nakata[001](ZCS(2.0)) 2018/01/11 : Change "About" information.
 *
 ************************************************/
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef PCDEBUG
#include "TzcsUtil.h"
#else
#include <tzcslib/TzcsUtil.h>
#endif

#include "tzcsI18N.h"
#include <locale.h>
#include "common.h"

/*--------------------------------------------------------------------
 * Global Definitions
 *------------------------------------------------------------------*/
#define DEBUG               0

/*--------------------------------------------------------------------
 * Resouces
 *------------------------------------------------------------------*/
char aboutLabel[512];
#define COPYRIGHT    "<b>Copyright © %s Toshiba Client Solutions Co., Ltd.</b>"
#define TMZC_VERSION "TMZC version: %s"

/*--------------------------------------------------------------------
 * Commands
 *------------------------------------------------------------------*/
const char *getcpuprocessor = "cat /proc/cpuinfo | awk -F': ' '/model name/ {print $2}' | head -n1";
const char *getmeminfor = "free -m | grep 'Mem' | awk '{print $2}'";
const char *getkernelversion = "uname -r";

/*--------------------------------------------------------------------
 * Global Data
 *------------------------------------------------------------------*/
void get_pixel_from_label(GtkWidget *label, const gchar *font,
        gint *width, gint *height);

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
static char * get_str_from_command (const char *cmd) {
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

GtkWidget *TzcsAbout(void) {
    GtkWidget *vbox;
    GtkWidget *inforbox;
    GtkWidget *about_label;
    GtkWidget *tmzc_label;
    GtkWidget *module_label;
    GtkWidget *protocol_label;
    GtkWidget *build_label;
    GtkWidget *bios_label;
//nakata add [001]
    FILE *fp;
    char tmzc_version_display[512]; 
//    char tmzc_version[512]    = N_("TMZC version: 1.2");
    char tmzc_version[512]    = N_("TMZC version:");
    char tmzc_version_value[512];
    char *TmzcVersion_file = "/usr/share/controlpanel/resource/TmzcVersion";
//nakata end [001]
    char module_version[512]  = N_("System module version");
    char vdi_protocol[512]    = N_("VDI Protocol");
    char build_id[512]        = N_("Build ID");
    char bios_version[512]    = N_("BIOS version");
    char year[10];   /* current year as string */
    ULONG hci_ret;
    int lwidth, lheight;
    int xbase, ybase;

    /* Create PackingBox */
    vbox = gtk_vbox_new (FALSE, 0);

    /* Create about label */
    about_label = gtk_label_new (NULL);
    FPRINT_DATETIME(year, "%Y");
    sprintf(aboutLabel, COPYRIGHT, year);
    gtk_label_set_markup (GTK_LABEL(about_label), aboutLabel);

    /* Table contains informations */
    inforbox = gtk_table_new (568, 708, TRUE);

    char infotmp[512];
    char tmp[512];

 //   /* TMZC version creation */
 //   sprintf(tmzc_version, TMZC_VERSION, "1.2");
 //   tmzc_label = gtk_label_new (_(tmzc_version));

    /* System module version creation */
    memset (infotmp, '\0', sizeof (infotmp));
    memset (tmp, '\0', sizeof (tmp));
#ifdef PCDEBUG
    strcat (infotmp, "version XX");
    hci_ret = HCI_SUCCESS;
#else
    hci_ret = get_hci_string (VAL_FTP_DATANAME, infotmp);
#endif
    sprintf(tmp, "%s : %s", _(module_version), infotmp);
    strcat (module_version, infotmp);

    /* TMZC version creation */
//nakata add [001]
  memset(tmzc_version_value, '\0', sizeof(tmzc_version_value));
  if ((fp = fopen(TmzcVersion_file, "r")) != NULL) {
    while (fgets((char *)tmzc_version_value, sizeof(tmzc_version_value), fp) != NULL) {
	tmzc_version_value[strlen(tmzc_version_value) - 1] = '\0';
    }
    fclose(fp);
  }
//    tmzc_label = gtk_label_new (_(tmzc_version));
    sprintf(tmzc_version_display, "%s %s (%s)", _(tmzc_version), tmzc_version_value, infotmp);
    tmzc_label = gtk_label_new (tmzc_version_display);
//nakata end [001]

#ifdef DEBUG
    if (hci_ret == HCI_FAIL) {
        g_print ("Get module version fail!\n");
    }
#endif
    module_label = gtk_label_new (tmp);

    memset (tmp, '\0', sizeof (tmp));
    /* VDI protocol creation */
    if (strstr (module_version, "VM") != NULL) {
        sprintf (tmp, "%s : %s", _(vdi_protocol), "VMware PCoIP");
    } else if (strstr (module_version, "CX") != NULL) {
        sprintf (tmp, "%s : %s", _(vdi_protocol), "Citrix ICA");
    } else if (strstr (module_version, "MS") != NULL) {
        sprintf (tmp, "%s : %s", _(vdi_protocol), "Microsoft RDP");
    } else {
        sprintf(tmp, "%s :", _(vdi_protocol));
    }
    protocol_label = gtk_label_new (tmp);

    memset (tmp, '\0', sizeof (tmp));
    /* Build ID creation */
#ifdef PCDEBUG
    sprintf (tmp, "%s : %s", _(build_id), "test");
#else
    sprintf (tmp, "%s : %s", _(build_id),
            get_str_from_command ("cat /etc/TZCS/version.txt"));
#endif
    build_label = gtk_label_new (tmp);

    /* BIOS version creation */
    memset (tmp, '\0', sizeof (tmp));
    memset (infotmp, '\0', sizeof (infotmp));
#ifdef PCDEBUG
    strcat(infotmp, "version YY");
    hci_ret = HCI_SUCCESS;
#else
    hci_ret = get_hci_string (VAL_BIOS_VER, infotmp);
#endif
    sprintf(tmp, "%s : %s", _(bios_version), infotmp);
#ifdef DEBUG
    if (hci_ret == HCI_FAIL) {
        g_print ("Get BIOS version fail!\n");
    }
#endif
    bios_label = gtk_label_new (tmp);

    gtk_widget_modify_font(GTK_WIDGET(about_label),
            pango_font_description_from_string("Sans 16"));
    xbase = 24;
    ybase = 20;
    get_pixel_from_label(about_label, (const gchar *)"Sans 16", &lwidth, &lheight);
    gtk_table_attach_defaults (GTK_TABLE (inforbox), about_label,
            xbase, xbase + lwidth,
            ybase, ybase + lheight);
    xbase = 90;
    ybase = ybase + lheight + 64;
    gtk_widget_modify_font(GTK_WIDGET(tmzc_label),
            pango_font_description_from_string("Sans 15"));
    get_pixel_from_label(tmzc_label, (const gchar *)"Sans 15", &lwidth, &lheight);
    gtk_table_attach_defaults (GTK_TABLE (inforbox), tmzc_label,
            xbase, xbase + lwidth,
            ybase, ybase + lheight);
    ybase = ybase + lheight + 24;
    gtk_widget_modify_font(GTK_WIDGET(bios_label),
            pango_font_description_from_string("Sans 15"));
    get_pixel_from_label(bios_label, (const gchar *)"Sans 15", &lwidth, &lheight);
    gtk_table_attach_defaults (GTK_TABLE (inforbox), bios_label,
            xbase, xbase + lwidth,
            ybase, ybase + lheight);
    ybase = ybase + lheight + 24;
    gtk_widget_modify_font(GTK_WIDGET(build_label),
            pango_font_description_from_string("Sans 15"));
    get_pixel_from_label(build_label, (const gchar *)"Sans 15", &lwidth, &lheight);
    gtk_table_attach_defaults (GTK_TABLE (inforbox), build_label,
            xbase, xbase + lwidth,
            ybase, ybase + lheight);
//    ybase = ybase + lheight + 24;
//    gtk_widget_modify_font(GTK_WIDGET(module_label),
//            pango_font_description_from_string("Sans 15"));
//    get_pixel_from_label(module_label, (const gchar *)"Sans 15", &lwidth, &lheight);
//    gtk_table_attach_defaults (GTK_TABLE (inforbox), module_label,
//            xbase, xbase + lwidth,
//            ybase, ybase + lheight);
//    ybase = ybase + lheight + 24;
//    gtk_widget_modify_font(GTK_WIDGET(protocol_label),
//            pango_font_description_from_string("Sans 15"));
//    get_pixel_from_label(protocol_label, (const gchar *)"Sans 15", &lwidth, &lheight);
//    gtk_table_attach_defaults (GTK_TABLE (inforbox), protocol_label,
//            xbase, xbase + lwidth,
//            ybase, ybase + lheight);

    gtk_box_pack_start (GTK_BOX (vbox), inforbox, FALSE, FALSE, 0);
    return vbox;
}
