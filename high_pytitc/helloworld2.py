#!/usr/bin/env python

# example helloworld2.py

import sys
sys.path.append('/usr/local/lib/python2.6/dist-packages/usb/')
import usb.core
import usb.util
import threading

import pygtk
pygtk.require('2.0')
import gobject
import gtk
import time
import array
import struct

gobject.threads_init()

class HelloWorld2:

    # Our new improved callback.  The data passed to this method
    # is printed to stdout.

    def poll_sensor(self):
        print "in thread"
        print "hw.exitflag = " + str(self.exitflag)
        ret = 0
        while not self.exitflag:
            self.velocity = self.velocity+self.velmult*self.acceleration
            self.steerangle = self.steerangle+self.steermult*self.steerspeed
            print ("angle = "+str(self.steerangle) + "velocity = " + str(self.velocity))
#            vel = array.array("B", [int(self.velocity)])
#            print (vel)
            vel = struct.pack("H", self.velocity)
            ang = struct.pack("H", self.steerangle)
#            print ("vel = " + str(vel))
#            print("len(vel)" + str(len(vel)))
            ret = self.dev.ctrl_transfer(0x01, 0x0, 0x05, 0, vel);
            ret = self.dev.ctrl_transfer(0x01, 0x0, 0x02, 0, ang);
#            print ("retw = " + str(ret))
#	    r = libusb_control_transfer(devh, CTRL_IN, USB_RQ_STAT, 0x06, 0, &stat22, 2, 0);
#            ret = self.dev.ctrl_transfer(0x81, 0x0, 0x06, 0, 2);
#            print ("retr = " + str(ret))

#            if ret == 0:
#                self.label.set_text("sunny")
#            else:
#                self.label.set_text("rainy")
            curtime = time.time()
            if ((curtime-self.lastpressed[0]>self.maxpresseddiff) and (curtime-self.lastpressed[3]>self.maxpresseddiff)):
        #right
                self.steermult=0.0
                self.steerangle=self.defsteerangle
#                print "releasing right"
            if ((curtime-self.lastpressed[1]>self.maxpresseddiff) and (curtime-self.lastpressed[2]>self.maxpresseddiff)):
        #forward
                self.velmult=0.0
                self.velocity=self.defvelocity
#                print "releasing forward"

            time.sleep(0.001)
        print "exiting thread"

    def key_pressed_event_cb(self, widget, event):
        print(event)
        if (event.keyval==65363):
        #right
            self.steermult=1.0
            self.lastpressed[0]=time.time()
        if (event.keyval==65362):
        #forward
            self.velmult=-1.0
            self.lastpressed[1]=time.time()
        if (event.keyval==65364):
        #reverse
            self.velmult=1.0
            self.lastpressed[2]=time.time()
        if (event.keyval==65361):
        #left
            self.steermult=-1.0
            self.lastpressed[3]=time.time()
        return True

    def callback(self, widget, data):

        self.label.set_text("connecting")
        self.dev = usb.core.find(idVendor=0xEB03, idProduct=0x0920)

        # was it found?
        if self.dev is None:
            raise ValueError('Device not found')

        self.devinfo.set_text("idVendor=0xEB03, idProduct=0x0920")
        self.label.set_text("found device")
        
        self.dev.set_configuration()

#        CTRL_OUT = [0x81, 0x0]
        self.dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, [0])

        print "starting thread"
        self.thread.start()
        print "thread started"
#        self.label.set_text("connected")
        return True

    # another callback
    def delete_event(self, widget, event, data=None):
        self.exitflag=1
        self.thread.join()
        gtk.main_quit()
        return False

    def __init__(self):
        self.velmult=0.0
        self.steermult=0.0
        self.steerspeed=1
        self.acceleration=0.1
        self.defvelocity=375.0
        self.defsteerangle=380.0        
        self.velocity=self.defvelocity
        self.steerangle=self.defsteerangle
        self.lastpressed=[0.0, 0.0, 0.0, 0.0]
        self.maxpresseddiff=0.1
        
        self.txt = None
        # Create a new window
        self.thread=threading.Thread(target=self.poll_sensor)
        self.exitflag=0
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

        # This is a new call, which just sets the title of our
        # new window to "Hello Buttons!"
        self.window.set_title("Rain sensor gui")

        # Here we just set a handler for delete_event that immediately
        # exits GTK.
        self.window.connect("delete_event", self.delete_event)

        # Sets the border width of the window.
        self.window.set_border_width(10)

        # We create a box to pack widgets into.  This is described in detail
        # in the "packing" section. The box is not really visible, it
        # is just used as a tool to arrange widgets.
        self.box1 = gtk.VBox(False, 5)

        # Put the box into the main window.
        self.window.add(self.box1)

        # Creates a new button with the label "Button 1".
        self.button1 = gtk.Button("Connect")

        # Now when the button is clicked, we call the "callback" method
        # with a pointer to "button 1" as its argument
        self.button1.connect("clicked", self.callback, "button 1")
        self.window.connect("key_press_event",self.key_pressed_event_cb)
#        self.window.connect("key_release_event",self.key_released_event_cb)

        frame = gtk.Frame("Sensor state:")
        self.label = gtk.Label("unknown")
        frame.add(self.label)

        frame2 = gtk.Frame("Device info:")
        self.devinfo = gtk.Label("unknown")
        frame2.add(self.devinfo)
        
        self.box1.pack_start(frame, False, False, 0)
        self.box1.pack_start(frame2, False, False, 0)

        # Instead of add(), we pack this button into the invisible
        # box, which has been packed into the window.
        self.box1.pack_start(self.button1, True, True, 0)

        # Always remember this step, this tells GTK that our preparation for
        # this button is complete, and it can now be displayed.
        self.button1.show()


        # The order in which we show the buttons is not really important, but I
        # recommend showing the window last, so it all pops up at once.
        self.box1.show()
        self.label.show()
        frame.show()
        self.devinfo.show()
        frame2.show()
        self.window.show()

def main():
    gtk.gdk.threads_init()
#    gtk.threads_enter()
    gtk.main()
#    gtk.threads_leave()

if __name__ == "__main__":
    hello = HelloWorld2()
    main()
