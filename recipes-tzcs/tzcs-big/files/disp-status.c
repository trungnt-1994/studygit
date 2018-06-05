#include <gtk/gtk.h>
#include <string.h>

#if 0
// for GetText //
#include <libintl.h>
#define _(String)gettext(String)
#ifdef gettext_noop
 #define N_(String) gettext_noop(String)
#else
 #define N_(String)(String)
#endif
#endif /* 0 */

///// 表示フォントサイズ /////
#define DISP_FONT_SIZE			"medium"

///// 表示制御時間定義 /////
//#define TIME_START_DISP_OFF		50
//#define TIME_STOP_DISP			60
#define TIME_ERASE_DISP			5000

///// 画面右下からのウィンドウの表示オフセット値 /////
#define DISP_SHIFT_X			10
#define DISP_SHIFT_Y			50



/*static*/ gboolean disp_timeout(gpointer usersetlocale_data)
{
	gtk_main_quit();

	return TRUE;
}


int main(int argc, char *argv[]) {

	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *label;

	gint width, height;

	char *output;
	char msg_to_disp[200];


//	setlocale(LC_ALL, "");
//	textdomain("disp-status");
	
	gtk_init(&argc, &argv);

	///// 引数チェック /////
	if (argc <= 1) {
		printf("Argument is missing!!\n");
		return 0;
	}

	///// set text /////
	strcpy(msg_to_disp, argv[1]);		// 1行目

//	if (argc >= 3) {
//		strcat(msg_to_disp, "\n");
//		strcat(msg_to_disp, argv[2]);	// 2行目
//	}

//	if (argc >= 4) {
//		strcat(msg_to_disp, "\n");
//		strcat(msg_to_disp, argv[3]);	// 3行目
//	}
	
	///// create window /////
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//	gtk_window_set_title(GTK_WINDOW(window), "Notice!!");
	gtk_window_set_title(GTK_WINDOW(window), "");

	///// create vbox /////
	vbox = gtk_vbox_new(FALSE/*TRUE*/, 15);
	gtk_container_add (GTK_CONTAINER (window), vbox);
//	gtk_box_set_spacing (GTK_BOX(vbox), 30);
			
	/// create label /////
	label = gtk_label_new(""/*"Wait a moment..."*/);
	output = g_markup_printf_escaped ("<span style=\"italic\" size=\"%s\">%s</span>" ,DISP_FONT_SIZE ,msg_to_disp);
	gtk_label_set_markup(GTK_LABEL(label), output);
	g_free(output);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);


	///// 共通処理 /////
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);

	/// イベントハンドラ登録 ///
	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), G_OBJECT(window));
//	g_signal_connect(G_OBJECT(window), "response",
//		G_CALLBACK(gtk_main_quit), G_OBJECT(window));

//	g_timeout_add (100,	disp_timeout,	window);
	g_timeout_add (TIME_ERASE_DISP,	disp_timeout,	window);

	/// 表示実行 ///
	gtk_widget_show_all(window);

	/// 表示位置移動（showした後でないとwindowの大きさが確定しないため、この位置で実施する） ///
	gtk_window_get_size(GTK_WINDOW(window), &width, &height);
//	gtk_window_move(GTK_WINDOW(window), gdk_screen_width() - (width + 10), gdk_screen_height() - (height + 50));
	gtk_window_move(GTK_WINDOW(window), gdk_screen_width() - (width + DISP_SHIFT_X), gdk_screen_height() - (height + DISP_SHIFT_Y));
	
	gtk_main();

	return 0;
}