#!/usr/bin/env python

import getopt

import sys
sys.path.append('/usr/local/lib/python2.6/dist-packages/usb/')
import usb.core
import usb.util
import threading
import socket


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

def usage():
    print ("rapidclient.py -h")
    print ("prints this page")
    print ("rapidclient.py -v")
    print ("prints version")
    print ("rapidclient.py --host=\"destination\" [-p port]")
    print ("rapidclient.py --host=\"destination\" [--port=port]")

def printversion():
    print ("unstable")


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
sock = None
stop = 0
#keycode, updateflag, value
#value 1 - pressed
#value 2 - released
updatekeys = {"left": [100,0,0], 
              "right": [102,0,0], 
              "up": [101,0,0], 
              "down": [103,0,0], 
              "1": [49,0,0], 
              "2": [50,0,0], 
              "3": [51,0,0]}
telemetry = [0,0,0,0,0,0,0,0]
thread = None
def intercom():
# update keys
    global stop
    global telemetry
    global sock
    global updatekeys
    while (not stop):
        for k, v in updatekeys.iteritems():
            if v[1]==1:
                v[1]=0
                packet = struct.pack("<B", v[2])
                sock.send(packet)
                packet = struct.pack("<B", v[0])
                sock.send(packet)
#                received = sock.recv(1)
#                hdr = struct.unpack("<B", received)
#receive telemetry
        packet = struct.pack("<B", 3)
        sock.send(packet)
        received = sock.recv(1)
        hdr = struct.unpack("<B", received)
        if hdr[0] == 254:
            i = 0
            while i<8:
                received = sock.recv(4)
                telemetry[i] = (struct.unpack("I", received))[0]
                i=i+1
            print telemetry

    sock.close()


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
    global sock
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))


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


# The function called whenever a key is pressed. Note the use of Python tuples to pass in: (key, x, y)  
def keyPressed(*args):
    # If escape is pressed, kill everything.
    global stop
    global updatekeys
    global thread
    print args[0] + " pressed"
    if args[0] == ESCAPE:
        stop = 1
        thread.join()
        glutDestroyWindow(window)
        sys.exit()
    if (args[0]=='1'):
        updatekeys["1"][1]=1
        updatekeys["1"][2]=1
    if (args[0]=='2'):
        updatekeys["2"][1]=1
        updatekeys["2"][2]=1
    if (args[0]=='3'):
        updatekeys["3"][1]=1
        updatekeys["3"][2]=1

def key_released(*args):
	# If escape is pressed, kill everything.
    global updatekeys
    print args[0] + " released"
    if (args[0]=='1'):
        updatekeys["1"][1]=1
        updatekeys["1"][2]=2
    if (args[0]=='2'):
        updatekeys["2"][1]=1
        updatekeys["2"][2]=2
    if (args[0]=='3'):
        updatekeys["3"][1]=1
        updatekeys["3"][2]=2

def special_pressed(*args):
	# If escape is pressed, kill everything.
    print str(args[0]) + " pressed"
    global updatekeys
    if (args[0]==101):
        #forward
        updatekeys["up"][1]=1
        updatekeys["up"][2]=1
    if (args[0]==103):
        #reverse
        updatekeys["down"][1]=1
        updatekeys["down"][2]=1
    if (args[0]==100):
    #left
        updatekeys["left"][1]=1
        updatekeys["left"][2]=1
    if (args[0]==102):
    #right
        updatekeys["right"][1]=1
        updatekeys["right"][2]=1

def special_released(*args):
	# If escape is pressed, kill everything.
    print str(args[0]) + " released"
    global updatekeys
    if (args[0]==101):
        #forward
        updatekeys["up"][1]=1
        updatekeys["up"][2]=2
    if (args[0]==103):
        #reverse
        updatekeys["down"][1]=1
        updatekeys["down"][2]=2
    if (args[0]==100):
    #left
        updatekeys["left"][1]=1
        updatekeys["left"][2]=2
    if (args[0]==102):
    #right
        updatekeys["right"][1]=1
        updatekeys["right"][2]=2

HOST, PORT = "0.0.0.0", 30000

def main():
    global HOST
    global PORT
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'p:', ['host=', 'port='])
    except getopt.GetoptError, err:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
        
    for o, a in optlist:
        if o == "-v":
            printversion()
            sys.exit()
        elif o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-p", "--port"):
            PORT=a
        elif o in ("--host"):
            HOST=a
        else:
            assert False, "unhandled option"
            
    if HOST=="0.0.0.0":
        usage()
        sys.exit()
        
    print "connecting to " + str(HOST) + ":" + str(PORT)
    
    global window
    global thread
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
    thread=threading.Thread(target=intercom)
    thread.start()
    
    # Start Event Processing Engine	
    glutMainLoop()

# Print message to console, and kick off the main to get it rolling.
print "Hit ESC key to quit."
main()
    	
