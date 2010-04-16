#!/usr/bin/env python

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

from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import sys
# Some api in the chain is translating the keystrokes to this octal string
# so instead of saying: ESCAPE = 27, we use the following.
ESCAPE = '\033'

# Number of the glut window.
window = 0

velmult=0.0
steermult=0.0
steerspeed=1
acceleration=0.1
defvelocity=375.0
defsteerangle=382.0        
velocity=defvelocity
steerangle=defsteerangle
lastpressed=[0.0, 0.0, 0.0, 0.0]
maxpresseddiff=0.1
dev=None
light1=0
light2=0
light3=0


# A general OpenGL initialization function.  Sets all of the initial parameters. 
def InitGL(Width, Height):				# We call this right after our OpenGL window is created.
    glClearColor(0.0, 0.0, 0.0, 0.0)	# This Will Clear The Background Color To Black
    glClearDepth(1.0)					# Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS)				# The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST)				# Enables Depth Testing
    glShadeModel(GL_SMOOTH)				# Enables Smooth Color Shading
	
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()					# Reset The Projection Matrix
										# Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0, float(Width)/float(Height), 0.1, 100.0)

    glMatrixMode(GL_MODELVIEW)
    global dev
    dev = usb.core.find(idVendor=0xEB03, idProduct=0x0920)
    
        # was it found?
    if dev is None:
        raise ValueError('Device not found')
    
    dev.set_configuration()
    
    #       CTRL_OUT = [0x81, 0x0]
    led = struct.pack("BB", 1, 1)
    dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
    time.sleep(0.2)
    led = struct.pack("BB", 1, 0)
    dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
    time.sleep(0.2)
    led = struct.pack("BB", 1, 1)
    dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
    time.sleep(0.2)
    led = struct.pack("BB", 1, 0)
    dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)


# The function called when our window is resized (which shouldn't happen if you enable fullscreen, below)
def ReSizeGLScene(Width, Height):
    if Height == 0:						# Prevent A Divide By Zero If The Window Is Too Small 
	    Height = 1

    glViewport(0, 0, Width, Height)		# Reset The Current Viewport And Perspective Transformation
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45.0, float(Width)/float(Height), 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)

# The main drawing function. 
def DrawGLScene():
	# Clear The Screen And The Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	glLoadIdentity()					# Reset The View 

	# Move Left 1.5 units and into the screen 6.0 units.
	glTranslatef(-1.5, 0.0, -6.0)

	# Draw a triangle
#	glBegin(GL_POLYGON)                 # Start drawing a polygon
#	glVertex3f(0.0, 1.0, 0.0)           # Top
#	glVertex3f(1.0, -1.0, 0.0)          # Bottom Right
#	glVertex3f(-1.0, -1.0, 0.0)         # Bottom Left
#	glEnd()                             # We are done with the polygon


	# Move Right 3.0 units.
#	glTranslatef(3.0, 0.0, 0.0)

	# Draw a square (quadrilateral)
#	glBegin(GL_QUADS)                   # Start drawing a 4 sided polygon
#	glVertex3f(-1.0, 1.0, 0.0)          # Top Left
#	glVertex3f(1.0, 1.0, 0.0)           # Top Right
#	glVertex3f(1.0, -1.0, 0.0)          # Bottom Right
#	glVertex3f(-1.0, -1.0, 0.0)         # Bottom Left
#	glEnd()                             # We are done with the polygon

	#  since this is double buffered, swap the buffers to display what just got drawn. 
        glutSwapBuffers()
        global velmult
        global steermult
        global steerspeed
        global acceleration
        global defvelocity
        global defsteerangle
        global velocity
        global steerangle
        global dev
        velocity = velocity+velmult*acceleration
        steerangle = steerangle+steermult*steerspeed
        if (velocity >= 400):
            velocity = 400
        if (velocity <= 365):
            velocity = 365
        if(steerangle > 500):
            steerangle = 500
        if(steerangle < 293):
            steerangle = 293
#        print ("angle = "+str(steerangle) + "velocity = " + str(velocity) + str(velmult)+str(acceleration))
        vel = struct.pack("H", velocity)
        ang = struct.pack("H", steerangle)
        ret = dev.ctrl_transfer(0x01, 0x0, 0x05, 0, vel);
        ret = dev.ctrl_transfer(0x01, 0x0, 0x02, 0, ang);

        led = struct.pack("BB", 0, light1)
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
        led = struct.pack("BB", 1, light2)
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
        led = struct.pack("BB", 2, light3)
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
#        adcm = struct.pack("B", 0)
        adc = dev.ctrl_transfer(0x81, 0x0, 0x08, 0x0, 4*8)
#        print adc
        adcv1 = struct.pack("BBBB", adc[0], adc[1], adc[2], adc[3])
        adcv2 = struct.pack("BBBB", adc[4], adc[5], adc[6], adc[7])
        adcv3 = struct.pack("BBBB", adc[8], adc[9], adc[10], adc[11])
        adcv4 = struct.pack("BBBB", adc[12], adc[13], adc[14], adc[15])
        adcv5 = struct.pack("BBBB", adc[16], adc[17], adc[18], adc[19])
        adcv6 = struct.pack("BBBB", adc[20], adc[21], adc[22], adc[23])
        adcv7 = struct.pack("BBBB", adc[24], adc[25], adc[26], adc[27])
        adcv8 = struct.pack("BBBB", adc[28], adc[29], adc[30], adc[31])
        adcv1 = struct.unpack("I", adcv1)
        adcv2 = struct.unpack("I", adcv2)
        adcv3 = struct.unpack("I", adcv3)
        adcv4 = struct.unpack("I", adcv4)
        adcv5 = struct.unpack("I", adcv5)
        adcv6 = struct.unpack("I", adcv6)
        adcv7 = struct.unpack("I", adcv7)
        adcv8 = struct.unpack("I", adcv8)
        ctr = dev.ctrl_transfer(0x81, 0x0, 0x03, 0x0, 4)
        ctrv = struct.pack("BBBB", ctr[0], ctr[1], ctr[2], ctr[3])
        ctrv = struct.unpack("I", ctrv)
        print str(ctrv[0]) + ": adcv1 = " + str(adcv1[0]) + " adcv2 = " + str(adcv2[0]) + " adcv3 = " + str(adcv3[0]) + " adcv4 = " + str(adcv4[0]) + " adcv5 = " + str(adcv5[0]) + " adcv6 = " + str(adcv6[0]) + " adcv7 = " + str(adcv7[0]) + " adcv8 = " + str(adcv8[0])

# The function called whenever a key is pressed. Note the use of Python tuples to pass in: (key, x, y)  
def keyPressed(*args):
	# If escape is pressed, kill everything.
    global velmult
    global velocity
    global light1
    global light2
    global light3
    if args[0] == ESCAPE:
        led = struct.pack("BB", 0, 1)
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
        time.sleep(0.2)
        led = struct.pack("BB", 0, 0)
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
        time.sleep(0.2)
        led = struct.pack("BB", 0, 1)
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
        time.sleep(0.2)
        led = struct.pack("BB", 0, 0)
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
        glutDestroyWindow(window)
        sys.exit()
    if args[0] == ' ':
        velocity = 400
        velmult=0.0
    if args[0] == '1':
        light1=1
        print "1 pressed"
    if args[0] == '2':
        light2=1
    if args[0] == '3':
        light3=1

def key_released(*args):
	# If escape is pressed, kill everything.
    global velmult
    global velocity
    global defvelocity
    global light1
    global light2
    global light3
    if args[0] == ' ':
        velocity = defvelocity
    if args[0] == '1':
        light1=0
    if args[0] == '2':
        light2=0
    if args[0] == '3':
        light3=0


def special_pressed(*args):
	# If escape is pressed, kill everything.
    print str(args[0]) + " pressed"
    global velmult
    global steermult
    global defvelocity
    global defsteerangle

    if (args[0]==101):
        #forward
        velmult=-1.0
    if (args[0]==103):
        #reverse
        velmult=1.0
    if (args[0]==100):
        #left
        steermult=-1.0
    if (args[0]==102):
        #right
        steermult=1.0

def special_released(*args):
	# If escape is pressed, kill everything.
    print str(args[0]) + " released"
    global velmult
    global steermult
    global defvelocity
    global defsteerangle
    global steerangle
    global velocity


    if (args[0]==101):
        #forward
        velmult=0.0
        velocity=defvelocity
    if (args[0]==103):
        #reverse
        velmult=0.0
        velocity=defvelocity
    if (args[0]==100):
        #left
        steermult=0.0
        steerangle=defsteerangle
    if (args[0]==102):
        #right
        steermult=0.0
        steerangle=defsteerangle

def main():
	global window
	# For now we just pass glutInit one empty argument. I wasn't sure what should or could be passed in (tuple, list, ...)
	# Once I find out the right stuff based on reading the PyOpenGL source, I'll address this.
	glutInit(())

	# Select type of Display mode:   
	#  Double buffer 
	#  RGBA color
	# Alpha components supported 
	# Depth buffer
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH)
	
	# get a 640 x 480 window 
	glutInitWindowSize(400, 400)
	
	# the window starts at the upper left corner of the screen 
	glutInitWindowPosition(0, 0)
	
	# Okay, like the C version we retain the window id to use when closing, but for those of you new
	# to Python (like my, remember this assignment would make the variable local and not global
	# if it weren't for the global declaration at the start of main.
	window = glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99")

   	# Register the drawing function with glut, BUT in Python land, at least using PyOpenGL, we need to
	# set the function pointer and invoke a function to actually register the callback, otherwise it
	# would be very much like the C version of the code.	
	glutDisplayFunc (DrawGLScene)
	
	# Uncomment this line to get full screen.
	#glutFullScreen()

	# When we are doing nothing, redraw the scene.
	glutIdleFunc(DrawGLScene)
	
	# Register the function called when our window is resized.
	glutReshapeFunc (ReSizeGLScene)
	
	# Register the function called when the keyboard is pressed.  
        
        glutIgnoreKeyRepeat(1)
	glutKeyboardFunc (keyPressed)
	glutKeyboardUpFunc (key_released)
	glutSpecialFunc (special_pressed)
	glutSpecialUpFunc (special_released)

	# Initialize our window. 
	InitGL(640, 480)

	# Start Event Processing Engine	
	glutMainLoop()

# Print message to console, and kick off the main to get it rolling.
print "Hit ESC key to quit."
main()
    	
