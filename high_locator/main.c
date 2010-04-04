/* $Id: basics.c 4 2008-06-22 09:19:11Z rbock $ */
/* GtkDatabox - An extension to the gtk+ library
 * Copyright (C) 1998 - 2008  Dr. Roland Bock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <stdio.h>

#include <gtk/gtk.h>
#include <gtkdatabox.h>
#include <gtkdatabox_points.h>
#include <gtkdatabox_ruler.h>
#include <math.h>

#define POINTS 256
#define STEPS 50
#define BARS 25
#define MARKER 10

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <gtk/gtkmain.h>

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <libusb.h>

#define EP_INTR			(1 | LIBUSB_ENDPOINT_IN)
#define EP_DATA			(2 | LIBUSB_ENDPOINT_IN)
//#define CTRL_IN			(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)
#define CTRL_IN			(0x01 | LIBUSB_ENDPOINT_IN)
#define CTRL_OUT		(0x01 | LIBUSB_ENDPOINT_OUT)
#define USB_RQ_STAT			0x0
#define USB_RQ_FEATURE                  0x03
#define INTR_LENGTH		64
//#define USE_JOYSTICK

enum {
	MODE_INIT = 0x00,
	MODE_AWAIT_FINGER_ON = 0x10,
	MODE_AWAIT_FINGER_OFF = 0x12,
	MODE_CAPTURE = 0x20,
	MODE_SHUT_UP = 0x30,
	MODE_READY = 0x80,
};

enum {
	STATE_AWAIT_MODE_CHANGE_AWAIT_FINGER_ON = 1,
	STATE_AWAIT_IRQ_FINGER_DETECTED,
	STATE_AWAIT_MODE_CHANGE_CAPTURE,
	STATE_AWAIT_IMAGE,
	STATE_AWAIT_MODE_CHANGE_AWAIT_FINGER_OFF,
	STATE_AWAIT_IRQ_FINGER_REMOVED,
};

static struct libusb_device_handle *devh = NULL;

static int find_dpfp_device(void)
{
	devh = libusb_open_device_with_vid_pid(NULL, 0xeb03, 0x0920);
	return devh ? 0 : -EIO;
}


gfloat *X;
gfloat *Y;
GtkDataboxGraph *graph;
GdkColor color;
GtkWidget *box;
int a;
pthread_t thread;
 
gboolean thread_func(void *vptr_args)
{
    int i;

    unsigned short adcbuf[256];
    unsigned char part=0;
    unsigned char amp=0;
    unsigned char usbdataready = 0;
    unsigned int counter = 0;
    unsigned char numchannels;
    unsigned char z;
    int r;   
    
    /*    amp=0x0;

	  r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x2, 0, &amp, 1, 0);*/

    r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x03, 0, &counter, 4, 0);
    printf ("time : %u\n", counter);
    r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x07, 0, &usbdataready, 1, 0);
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
    //    printf ("checking wether usbdata is ready\n");
    if (usbdataready == 1)
      {
	//	printf ("data ready, reading\n");
	r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x04, 0, &numchannels, 1, 0);
	//	printf ("channel quantity request returned %i\n", numchannels);
	for (z = 0; z< numchannels; z++)
	{
	  //	  printf ("reading channel %i data\n",z);
	  r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x01, 0, &z, 1, 0);
	  part = 0;
	  r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x05, 0, &part, 1, 0);
	  r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x06, 0, (unsigned char *)adcbuf, 128, 0);
	  part = 1;
	  r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x05, 0, &part, 1, 0);
	  r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x06, 0, (unsigned char *)adcbuf+128, 128, 0);
	  part = 2;
	  r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x05, 0, &part, 1, 0);
	  r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x06, 0, (unsigned char *)adcbuf+256, 128, 0);
	  part = 3;
	  r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x05, 0, &part, 1, 0);
	  r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x06, 0, (unsigned char *)adcbuf+384, 128, 0);
	  if (z == 5)
	    {
	      gtk_databox_graph_remove (GTK_DATABOX (box), graph);
	      for (i=0; i<POINTS; i++)
		{
		  //		  printf ("%i ", adcbuf[i]);
		  X[i] = i;
		  Y[i] = adcbuf[i];
	
		}
	      //	      printf ("\n");
	    }

	}
      }

    graph = gtk_databox_lines_new (POINTS, X, Y, &color, 1);
    gtk_databox_graph_add (GTK_DATABOX (box), graph);
    //    gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);
    //	gtk_databox_calculate_extrema (GTK_DATABOX (box), &min_x, &max_x, &min_y, &max_y);
    
    gtk_widget_queue_draw (GTK_WIDGET(box));
    
    return TRUE;
}


/*----------------------------------------------------------------
 *  databox basics
 *----------------------------------------------------------------*/

static void
create_basics (void)
{
   GtkWidget *window = NULL;
   GtkWidget *vbox;
   GtkWidget *close_button;
   GtkWidget *label;
   GtkWidget *separator;
   GtkWidget *table;
   gint i;

   /* We define some data */
   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i;
      Y[i] = 0;
   }

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_set_size_request (window, 500, 500);

   g_signal_connect (GTK_OBJECT (window), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);

   gtk_window_set_title (GTK_WINDOW (window), "GtkDatabox: Basics");
   gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), vbox);

   label =
      gtk_label_new
      ("\nplot data.\n\nUsage:\nDraw a selection with the left button pressed,\nThan click into the selection.\nUse the right mouse button to zoom out.\nShift+ right mouse button zooms to default.");
   gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

   /* ----------------------------------------------------------------- 
    * This is all you need:
    * -----------------------------------------------------------------
    */

   /* Create the GtkDatabox widget */
   gtk_databox_create_box_with_scrollbars_and_rulers (&box, &table,
						      TRUE, TRUE, TRUE, TRUE);

   /* Put it somewhere */
   gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);

   /* Add your data data in some color */
   color.red = 0;
   color.green = 0;
   color.blue = 0;

   graph = gtk_databox_lines_new (POINTS, X, Y, &color, 1);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   gtk_databox_set_total_limits (GTK_DATABOX (box), -100., 356.0, 3400.0, -100.);
   gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);

   /* ----------------------------------------------------------------- 
    * Done :-)
    * -----------------------------------------------------------------
    */

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);

   close_button = gtk_button_new_with_label ("close");
   g_signal_connect_swapped (GTK_OBJECT (close_button), "clicked",
			     G_CALLBACK (gtk_main_quit), GTK_OBJECT (box));
   gtk_box_pack_start (GTK_BOX (vbox), close_button, FALSE, FALSE, 0);
   GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
   gtk_widget_grab_default (close_button);
   gtk_widget_grab_focus (close_button);


   g_timeout_add(300, thread_func, NULL);

   gtk_widget_show_all (window);
   gdk_window_set_cursor (box->window, gdk_cursor_new (GDK_CROSS));
}

gint
main (gint argc, char *argv[])
{
  //  struct sigaction sigact;
  int r = 1;
  
  r = libusb_init(NULL);
  if (r < 0) {
    fprintf(stderr, "failed to initialise libusb\n");
    exit(1);
  }

  r = find_dpfp_device();
  if (r < 0) {
    fprintf(stderr, "Could not find/open device\n");
    goto out;
  }
  
  r = libusb_claim_interface(devh, 0);
  if (r < 0) {
    fprintf(stderr, "usb_claim_interface error %d\n", r);
    goto out;
  }
  printf("claimed interface\n");
  
  a=0;

  gtk_init (&argc, &argv);
  
  create_basics ();
  gtk_main ();

  printf("shutting down...\n");

  libusb_release_interface(devh, 0);
 out:
  libusb_close(devh);
  libusb_exit(NULL);
  return r >= 0 ? r : -r;


  return 0;
}
