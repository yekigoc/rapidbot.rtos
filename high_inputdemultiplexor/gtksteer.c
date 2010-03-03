#include <gtk/gtk.h>

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <libusb.h>


#define EP_INTR			(1 | LIBUSB_ENDPOINT_IN)
#define EP_DATA			(2 | LIBUSB_ENDPOINT_IN)
//#define CTRL_IN			(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)
#define CTRL_IN			(0x01 | LIBUSB_ENDPOINT_IN)
#define CTRL_OUT		(0x01 | LIBUSB_ENDPOINT_OUT)
#define USB_RQ_STAT			0x0
#define USB_RQ_FEATURE                  0x03
#define INTR_LENGTH		64

static struct libusb_device_handle *devh = NULL;

static int find_dpfp_device(void)
{
	devh = libusb_open_device_with_vid_pid(NULL, 0xeb03, 0x0920);
	return devh ? 0 : -EIO;
}

static int get_hwstat(unsigned char *status)
{
	int r;
	unsigned short stat = 490;
	unsigned short stat2 = 490;
	unsigned int stat1 = 490;
	unsigned int spistat = 0;
	unsigned int ctr = 0;
	int m=1;
	printf ("get_hwstat\n");
	while (1)
	  {
	    r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x01, 0, &stat2, 2, 0);
	    if (r < 0) 
	      {
		fprintf(stderr, "set hwstat error %d\n", r);
		return r;
	      }
	    if ((unsigned int) r < 1) 
	      {
		fprintf(stderr, "short write (%d)", r);
		return -1;
	      }
	    
	    r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x02, 0, &stat, 2, 0);

	    r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x03, 0, &ctr, 4, 0);
	    if (r < 0) 
	      {
		fprintf(stderr, "set hwstat error %d\n", r);
		return r;
	      }
	    if ((unsigned int) r < 1) 
	      {
		fprintf(stderr, "short write (%d) 2", r);
		return -1;
	      }
	    
	    stat1 = stat1+m;
	    stat = stat1;

	    if (stat==499)
	      m=-1;
	    if (stat==490)
	      m=1;

	    printf("hwstat counter=%i dutycycle = %i\n", ctr, stat2);

	    usleep(100000);
	  }
	return 0;
}

GtkWidget * text;

/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
static void hello( GtkWidget *widget,
                   gpointer   data )
{
    g_print ("Hello World\n");
    char *a = gtk_entry_get_text( text );
    int pwm;
    sscanf (a,"%i",&pwm);

    //    libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x02, 0, &pwm, 2, 0);
    libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x05, 0, &pwm, 2, 0);
}

static gboolean delete_event( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    g_print ("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete_event". */

    return FALSE;
}

/* Another callback */
static void destroy( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}

int main( int   argc,
          char *argv[] )
{
    GtkWidget *window;
    GtkWidget *button;

	struct sigaction sigact;
	int r = 1;

	r = libusb_init(NULL);
	if (r < 0) 
	  {
	    fprintf(stderr, "failed to initialise libusb\n");
	    exit(1);
	  }

	r = find_dpfp_device();
	if (r < 0) 
	  {
	    fprintf(stderr, "Could not find/open device\n");
	  }

	r = libusb_claim_interface(devh, 0);
	if (r < 0) 
	  {
	    fprintf(stderr, "usb_claim_interface error %d\n", r);
	  }
	printf("claimed interface\n");
	
	//	get_hwstat(unsigned char *status);

    
    gtk_init (&argc, &argv);
    
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "delete_event",
		      G_CALLBACK (delete_event), NULL);
    
    g_signal_connect (G_OBJECT (window), "destroy",
		      G_CALLBACK (destroy), NULL);
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    
    button = gtk_button_new_with_label ("Hello World");
    
    g_signal_connect (G_OBJECT (button), "clicked",
		      G_CALLBACK (hello), NULL);

    GtkWidget *vbox;
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_show (vbox);

    text= gtk_entry_new();
    
    /* This will cause the window to be destroyed by calling
     * gtk_widget_destroy(window) when "clicked".  Again, the destroy
     * signal could come from here, or the window manager. */
    g_signal_connect_swapped (G_OBJECT (button), "clicked",
			      G_CALLBACK (hello),
                              G_OBJECT (window));
    
    /* This packs the button into the window (a gtk container). */
    gtk_box_pack_start (GTK_BOX (vbox), text, TRUE, TRUE, 0);
    gtk_widget_show (text);
    gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);    
    /* The final step is to display this newly created widget. */
    gtk_widget_show (button);
    
    /* and the window */
    gtk_widget_show (window);
    
    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();

    libusb_release_interface(devh, 0);
    
    libusb_close(devh);
    libusb_exit(NULL);

    return 0;
}
