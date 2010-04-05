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
#include <fix_fft.h>
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
gfloat *X_scope;
gfloat *Y_scope;
GtkDataboxGraph *graph;
GdkColor color;
GtkWidget *box;
GtkDataboxGraph *graph_scope;
GdkColor color_scope;
GtkWidget *box_scope;
int a;
pthread_t thread;
GtkWidget *spinner;
unsigned short adcbuf[256];

#define FFT_SIZE  256
#define log2FFT   8
#define N         (2 * FFT_SIZE)
#define log2N     (log2FFT + 1)

short x[N], fx[N];
typedef struct
{
  unsigned char ampchannel;
  unsigned char amp;
  unsigned char ampchanged;
} amp;

amp amp5;

typedef struct
{
  int val[5];
  int freq[5];
} maximumpoints;
maximumpoints maxpts;

#define MIDDLEPOINT 1650
#define MAX_AMPLITUDE 1000
#define MIN_AMPLITUDE 200

int findabsmax()
{
  int i = 0;
  int max = 0;
  for (i=0;i<256;i++)
    {
      if (abs(adcbuf[i]-MIDDLEPOINT)>max)
	max = adcbuf[i]-MIDDLEPOINT;
    }
  return max;
}

void find5maximumpoints()
{
  memset(&maxpts, 0, sizeof(maximumpoints));
  int i = 0;
  int j = 0;
  for (i=0; i<N/2;i++)
    {
      for (j=0;j<5;j++)
	{
	  if (maxpts.val[j]-abs(fx[i])<0)
	    {
	      maxpts.val[j] = abs(fx[i]);
	      maxpts.freq[j] = i;
	      break;
	    }
	}
    }
  for (j=0;j<5;j++)
    {
      printf ("mp%i = %i ", j, maxpts.freq[j]);
    }
  printf("\n");
}

gboolean set_gain(void *vptr_args)
{
  amp5.ampchannel = 5;
  amp5.amp = gtk_spin_button_get_value_as_int(spinner);
  amp5.ampchanged = 1;
  printf ("called set_gain\n");
  return TRUE;
}

gboolean thread_func(void *vptr_args)
{
    int i;
    unsigned char part = 0;
    unsigned char usbdataready = 0;
    unsigned int counter = 0;
    unsigned char numchannels;
    unsigned char z;
    unsigned short ampl = 0;
    int absmax;
    int r;   

    /*    amp=0x0;*/
	  
    if (amp5.ampchanged == 1)
      {
	ampl = 0;
	unsigned short samp = amp5.amp;
	ampl = amp5.ampchannel | (samp)<<8;
	printf ("amp = %i, ampchan = %i, setting ampl = %i\n",amp5.amp, amp5.ampchannel, ampl);
	r = libusb_control_transfer(devh, CTRL_OUT, USB_RQ_STAT, 0x2, 0, &ampl, 2, 0);
	amp5.ampchanged = 0;
      }

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
	      r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x08, 0, (unsigned char *)&amp5.amp, 1, 0);
	      printf ("amp5.amp = %i\n", amp5.amp);
		
	      gtk_spin_button_set_value(spinner, (double)amp5.amp);

	      gtk_databox_graph_remove (GTK_DATABOX (box), graph);
	      gtk_databox_graph_remove (GTK_DATABOX (box_scope), graph_scope);
	      for (i=0; i<POINTS; i++)
		{
		  //		  printf ("%i ", adcbuf[i]);
		  X[i] = i;
		  Y[i] = adcbuf[i];
		  x[i] = Y[i]-1600;
		  if (i & 0x01)
		    fx[(N+i)>>1] = x[i];
		  else
		    fx[i>>1] = x[i];
		}
	      
	      fix_fftr(fx, log2N, 0);
	      find5maximumpoints();
	      for (i=0; i<N; i++)
		{
		  X_scope[i] = i;
		  Y_scope[i] = ((float)fx[i])+1600.0;
		  //		  printf ("%f ", Y_scope[i]);
		}
	      //	      printf("\n");
		  
	      //	      printf ("\n");
	    }

	}
      }

    graph = gtk_databox_lines_new (POINTS, X, Y, &color, 1);
    gtk_databox_graph_add (GTK_DATABOX (box), graph);
    graph_scope = gtk_databox_lines_new (N, X_scope, Y_scope, &color_scope, 1);
    gtk_databox_graph_add (GTK_DATABOX (box_scope), graph_scope);

    //    gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);
    //	gtk_databox_calculate_extrema (GTK_DATABOX (box), &min_x, &max_x, &min_y, &max_y);
    
    gtk_widget_queue_draw (GTK_WIDGET(box));
    gtk_widget_queue_draw (GTK_WIDGET(box_scope));

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
   GtkWidget *setgain_button;
   GtkWidget *label;
   GtkWidget *separator;
   GtkWidget *table;
   GtkWidget *table_scope;
   GtkWidget *hbox;
   GtkAdjustment *adj;
  
   gint i;

   /* We define some data */
   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i;
      Y[i] = 0;
   }


   /* We define some data */
   X_scope = g_new0 (gfloat, N);
   Y_scope = g_new0 (gfloat, N);

   for (i = 0; i < N; i++)
   {
      X_scope[i] = i;
      Y_scope[i] = 0;
   }

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_set_size_request (window, 500, 700);

   g_signal_connect (GTK_OBJECT (window), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);

   gtk_window_set_title (GTK_WINDOW (window), "Locator scope");
   gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), vbox);

   label = gtk_label_new("scope");
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
   /* Create the GtkDatabox widget */
   gtk_databox_create_box_with_scrollbars_and_rulers (&box_scope, &table_scope,
						      TRUE, TRUE, TRUE, TRUE);

   /* Put it somewhere */
   gtk_box_pack_start (GTK_BOX (vbox), table_scope, TRUE, TRUE, 0);

   /* Add your data data in some color */
   color_scope.red = 0;
   color_scope.green = 0;
   color_scope.blue = 0;

   graph_scope = gtk_databox_lines_new (N, X_scope, Y_scope, &color_scope, 1);
   gtk_databox_graph_add (GTK_DATABOX (box_scope), graph_scope);

   gtk_databox_set_total_limits (GTK_DATABOX (box_scope), -10., 522.0, 2600.0, 600.);
   gtk_databox_auto_rescale (GTK_DATABOX (box_scope), 0.05);

   /* ----------------------------------------------------------------- 
    * Done :-)
    * -----------------------------------------------------------------
    */

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);
   hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

   //   gtk_container_add (GTK_CONTAINER (vbox), hbox);

   label = gtk_label_new ("Manual gain : ");
   gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
   adj = (GtkAdjustment *) gtk_adjustment_new (0,0, 15.0, 1.0,
					       1.0, 0.0);
   spinner = gtk_spin_button_new (adj, 0, 0);
   gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (spinner), TRUE);
   gtk_box_pack_start (GTK_BOX (hbox), spinner, FALSE, TRUE, 0);

   setgain_button = gtk_button_new_with_label ("Set");
   g_signal_connect_swapped (GTK_OBJECT (setgain_button), "clicked",
			     G_CALLBACK (set_gain), GTK_OBJECT (box));
   gtk_box_pack_start (GTK_BOX (hbox), setgain_button, FALSE, FALSE, 0);

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);

   close_button = gtk_button_new_with_label ("close");
   g_signal_connect_swapped (GTK_OBJECT (close_button), "clicked",
			     G_CALLBACK (gtk_main_quit), GTK_OBJECT (box));
   gtk_box_pack_start (GTK_BOX (vbox), close_button, FALSE, FALSE, 0);
   GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
   gtk_widget_grab_default (close_button);
   gtk_widget_grab_focus (close_button);

   g_timeout_add(500, thread_func, NULL);

   gtk_widget_show_all (window);
   gdk_window_set_cursor (box->window, gdk_cursor_new (GDK_CROSS));
   gdk_window_set_cursor (box_scope->window, gdk_cursor_new (GDK_CROSS));
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
