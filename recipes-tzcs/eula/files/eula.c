/*####################################################################################*/
/*# gcc `pkg-config --cflags --libs gtk+-2.0 poppler-glib` -o eula eula.c  #*/
/*####################################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <poppler.h>
#include <libintl.h>
#include <locale.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "tvalzctl.h"

#define POWER_OFF "poweroff"
#define START "\n\
nohup icewm-session & \n\
"
static PopplerDocument* doc;
static PopplerPage* page[100];
static void set_flag_to_bios();

int pages;
/*
  * Callback destroy when gtk_main_destroy is called
  */
static void on_destroy(GtkWidget* w, gpointer data) {
    gtk_main_quit();
}

/*
  * Draw content of :License to textview
  */
static gboolean on_expose(GtkWidget* w, GdkEventExpose* e, gpointer data) {
    cairo_t* cr;
    int i = GPOINTER_TO_INT(data);
    gtk_widget_realize(w);
    cr = gdk_cairo_create(w->window);
    poppler_page_render(page[i], cr);
    cairo_destroy(cr);
    return FALSE;
}
/*
  *  Callback function for press yes button
  */
static gboolean yes_pressed(GtkWidget *button,GdkEventButton *event, gpointer data) {
    gtk_widget_destroy(data);
    set_flag_to_bios();
    system(START);
    return  TRUE;
}
/*
 * Callback function for press no button
 */
static gboolean no_pressed(GtkWidget *button,GdkEventButton *event,gpointer data) {
    system(POWER_OFF);
    return  TRUE;
}
/*
 * Read flag from bios
 */
static int read_flag_from_bios() {
    int fd;

    int result;
    ghci_interface ghci_buf, ghci_buf_plus;
    fd = open("/dev/tvalz0", O_RDONLY);

    ghci_buf.ghci_eax = 0xFE00;
    ghci_buf.ghci_ebx = 0x00D0;
    ghci_buf.ghci_ecx = 0x0022; 
    ghci_buf.ghci_edx = 0x0000;
    ghci_buf.ghci_esi = 0x0000;
    ghci_buf.ghci_edi = 0x0000;

    ghci_buf_plus.ghci_eax = 0xFE00;
    ghci_buf_plus.ghci_ebx = 0x00D0;
    ghci_buf_plus.ghci_ecx = 0x002F;
    ghci_buf_plus.ghci_edx = 0x0000;
    ghci_buf_plus.ghci_esi = 0x0000;
    ghci_buf_plus.ghci_edi = 0x0000;			
    result = ioctl(fd, IOCTL_TVALZ_GHCI, &ghci_buf); // Read
    result = ioctl(fd, IOCTL_TVALZ_GHCI, &ghci_buf_plus); // Read 
    if ((ghci_buf.ghci_eax & 0xFF00) != 0 ||(ghci_buf_plus.ghci_eax & 0xFF00) != 0 ) {
       // write error
    } else {
            if((ghci_buf.ghci_edx & 0xff) == 0x17) {
                result = 1;
            } else {
		if(ghci_buf_plus.ghci_ecx == 0){
			// unapproved(ghci_buf.ghci_ecx F0)
			result = 0;		
		} else {
                	// approved(ghci_buf.ghci_ecx F1)
                	result = 1;
		} 

            }


    }
    close(fd);  
    return result;    
}
/*
 * Set flag to BIOS
 */
static void set_flag_to_bios() {
    int fd;
    int result;
    ghci_interface ghci_buf;
    fd = open("/dev/tvalz0", O_RDONLY);

    ghci_buf.ghci_eax = 0xFF00;
    ghci_buf.ghci_ebx = 0x00D0;
    ghci_buf.ghci_ecx = 0x002F;
    ghci_buf.ghci_edx = 0x0000;
    ghci_buf.ghci_esi = 0x0001;//(Set approved)@In the case of unapproved, You set g0x0000h.
    ghci_buf.ghci_edi = 0x0000;

    result = ioctl(fd,IOCTL_TVALZ_GHCI,&ghci_buf);
    if ((ghci_buf.ghci_eax & 0xFF00) != 0) {
       
    }

    close(fd);
}
/*
 * Create window to show license
 */
static int create_window(int argc, char* license[]){
    GtkWidget* win;
    GtkWidget* scrollwin;
    GtkWidget* vbox;
    GtkWidget* text_vbox;
    GtkWidget* textview[100];
    GtkWidget* hbox;
    GtkWidget* yes_button;
    GtkWidget* no_button;
    GdkColor color;
    gdk_color_parse("#FFFFFF",&color);

    GError* err = NULL;
    int i;
    if (argc != 2) {
        printf("Useage: pdfviewer <uri>\n");
        return 1;
    }

    gtk_init(&argc, &license);

    doc = poppler_document_new_from_file(license[1], NULL, &err);
    if (!doc) {
        printf("%s\n", err->message);
        g_object_unref(err);
        return 2;
    }
    pages = poppler_document_get_n_pages(doc);
    printf("There are %d pages in this pdf.\n", pages);
    if(!page) {
        printf("Could not open first page of document\n");
        g_object_unref(doc);
        return 3;
    }
     
    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_CENTER_ALWAYS);
    scrollwin = gtk_scrolled_window_new (NULL, NULL);
    vbox = gtk_vbox_new(FALSE,0);
    text_vbox = gtk_vbox_new(FALSE,0);
    hbox = gtk_hbox_new(FALSE,10);
    yes_button = gtk_button_new_with_label("Yes");
    no_button = gtk_button_new_with_label("No");
    gtk_widget_modify_bg(GTK_WIDGET(win),GTK_STATE_NORMAL,&color); 
    gtk_container_set_border_width (GTK_CONTAINER (scrollwin), 10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollwin),
                                    GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(win),vbox);
    for(i =0; i< pages;i++) {
        textview[i] = gtk_drawing_area_new();
        gtk_widget_set_size_request(GTK_WIDGET(textview[i]),600,1000);
        gtk_widget_modify_bg(GTK_WIDGET(textview[i]),GTK_STATE_NORMAL,&color);         
        page[i] = poppler_document_get_page(doc,i);
        if(i != 0 && i != (pages-1)){
            gtk_box_pack_start(GTK_BOX(text_vbox),textview[i],FALSE,FALSE,10);
        } else {
            gtk_box_pack_start(GTK_BOX(text_vbox),textview[i],FALSE,FALSE,0);           
        }
        gtk_widget_set_app_paintable(textview[i], TRUE);   
        g_signal_connect(G_OBJECT(textview[i]), "expose-event", G_CALLBACK(on_expose), GINT_TO_POINTER(i));        
    }
    gtk_scrolled_window_add_with_viewport (
                   GTK_SCROLLED_WINDOW (scrollwin), text_vbox);
    //Add checkbox and Ok yes_button to hbox
    gtk_box_pack_end(GTK_BOX(hbox), no_button,FALSE,FALSE, 5);
    gtk_box_pack_end(GTK_BOX(hbox),yes_button,FALSE,FALSE, 5);
    g_signal_connect(G_OBJECT(yes_button),"button-press-event",G_CALLBACK(yes_pressed),win);
    g_signal_connect(G_OBJECT(no_button),"button-press-event",G_CALLBACK(no_pressed),NULL);  
    //Add scrollwin and hbox to vbox
    gtk_box_pack_start(GTK_BOX(vbox),scrollwin,FALSE,FALSE,0);
    gtk_box_pack_end(GTK_BOX(vbox),hbox,FALSE,FALSE,10);
    gtk_window_set_decorated (GTK_WINDOW (win), FALSE);
    gtk_widget_set_size_request(GTK_WIDGET(scrollwin),570,660);  
    gtk_widget_set_size_request(GTK_WIDGET(win),600,700);
    gtk_widget_set_size_request(GTK_WIDGET(yes_button),60,30);
    gtk_widget_set_size_request(GTK_WIDGET(no_button),60,30);
    gtk_window_set_frame_dimensions(GTK_WINDOW(win), 10, 10, 10, 10);
    gtk_window_set_resizable (GTK_WINDOW(win), FALSE);
    g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(on_destroy), NULL);
    gtk_widget_set_app_paintable(win, TRUE);   
    gtk_widget_show_all(win);

    gtk_main();

    g_object_unref(page);
    g_object_unref(doc);
    return 0;
}
/*
  * Main function
  */
int main(int argc, char* argv[]) {
    if(read_flag_from_bios() == 0 ) {    
        create_window(argc,argv);
    } else {
      system(START);
    }
    return 0;
}
