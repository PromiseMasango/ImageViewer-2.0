#include <gtk/gtk.h>
#include <gdk/gdkscreen.h>
#include <cairo.h>
#include <string.h>

/*function prototype*/
static gboolean expose(GtkWidget *widget, cairo_t *palette, gpointer ptr);
static void open_file(GtkWidget *widget, gpointer ptr);


struct Application{
	/*window*/
	GtkWidget *window;

	/*icon*/
	GdkPixbuf *icon;

	/*main layout*/
	GtkWidget *main_vbox;

	/*image area*/
	GtkWidget *canvas;
	gboolean selected_image;
	char filename[256];


	/*control layout*/
	GtkWidget *button_hbox;
	GtkWidget *button_open;

	/*return values*/
	int result;
};

struct Application application;

int main( int argv, char *argc[] ){
	/*initialize gtk*/
	gtk_init( &argv, &argc );

	/*window*/
	application.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(application.window), "ImageViewer-3");
	gtk_window_set_default_size(GTK_WINDOW(application.window), 500, 500);
	gtk_window_set_position(GTK_WINDOW(application.window), GTK_WIN_POS_MOUSE);
	gtk_container_set_border_width(GTK_CONTAINER (application.window), 0);
	gtk_window_set_resizable(application.window, TRUE );

	/*icon*/
	GtkWidget *icon_image = gtk_image_new_from_file("background.png");
	application.icon = gtk_image_get_pixbuf( icon_image );
	gtk_window_set_icon( application.window, application.icon );

	/*main layout*/
	application.main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

	/*image area*/
	application.canvas = gtk_drawing_area_new();

	/*control layout*/
	application.button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	application.button_open = gtk_button_new_with_label("open file");

	/*pack widgets*/
	gtk_box_pack_start(application.button_hbox, application.button_open, TRUE, TRUE, 0 );

	gtk_box_pack_start(application.main_vbox, application.canvas, TRUE, TRUE, 0 );
	gtk_box_pack_start(application.main_vbox, application.button_hbox, FALSE, FALSE, 0 );

	/*listen to events*/
	g_signal_connect( application.window, "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect( application.canvas, "draw", G_CALLBACK(expose), NULL);
	g_signal_connect( application.button_open, "clicked", G_CALLBACK(open_file), NULL);


	/*add to container*/
	gtk_container_add( application.window, application.main_vbox);

	gtk_widget_show_all(application.window);
	gtk_main();
	return 0;
}

/*redraw a selected image on a DrawingArea*/
static gboolean expose(GtkWidget *widget, cairo_t *palette, gpointer ptr){
	cairo_set_operator(palette, CAIRO_OPERATOR_SOURCE);

	if( application.selected_image == FALSE ){
		cairo_surface_t *image = cairo_image_surface_create_from_png("background.png");

		 if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS){
			g_print("error, cannot load background image\n");
			exit(1);
		}

		cairo_set_source_surface(palette, image, 0, 0);

		cairo_paint(palette);
		cairo_destroy(image);
		return TRUE;
	}

	if( application.selected_image == TRUE ){

		cairo_surface_t *image = cairo_image_surface_create_from_png(application.filename);

		 if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS){
			g_print("error, cannot load background image\n");
			exit(1);
		}

		gfloat screen_width = gdk_screen_get_width (gdk_screen_get_default ());
		gfloat screen_height = gdk_screen_get_height (gdk_screen_get_default());

		gfloat image_width = cairo_image_surface_get_width(image);
		gfloat image_height = cairo_image_surface_get_height(image);

		gfloat scale_width = screen_width / image_width;
		gfloat scale_height = screen_height / image_width;

#ifdef DEBUG
		g_print("screen width :%f\n", screen_width );
		g_print("screen height :%f\n", screen_height );

		g_print("image width :%f\n", image_width );
		g_print("image height :%f\n", image_height );
#endif

		cairo_scale(palette, scale_width, scale_height);


		cairo_set_source_surface(palette, image, 0, 0);

		cairo_paint(palette);
		cairo_destroy(image);
		return TRUE;
	}

}

/*open popup window allowing a user to select a png image*/
static void open_file(GtkWidget *widget, gpointer ptr){
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Select image", application.window, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter,"*.png");

	gtk_file_chooser_add_filter( dialog, filter );

	application.result = gtk_dialog_run( GTK_DIALOG(dialog) );

	if( application.result == GTK_RESPONSE_ACCEPT){
		/*clear filename buffer*/
		memset( application.filename, 0, 256 );

		strncpy(application.filename, gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dialog) ), 255 );
		g_print("info: selected image ->%s\n", application.filename);
		application.selected_image = TRUE;
	}

	gtk_widget_destroy(dialog);
}

