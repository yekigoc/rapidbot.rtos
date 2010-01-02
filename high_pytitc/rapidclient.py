#!/usr/bin/env python

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

HOST, PORT = "10.218.35.4", 30000

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
    global velmult
    global velocity
    global sock
    if args[0] == ESCAPE:
        sock.close()
        glutDestroyWindow(window)
        sys.exit()
    if args[0] == ' ':
        velocity = 400
        velmult=0.0

def key_released(*args):
	# If escape is pressed, kill everything.
    global velmult
    global velocity
    global defvelocity
    if args[0] == ' ':
        velocity = defvelocity

def special_pressed(*args):
	# If escape is pressed, kill everything.
    print str(args[0]) + " pressed"
    global velmult
    global steermult
    global defvelocity
    global defsteerangle

    global sock
    packet = struct.pack("<B", 1)
    sock.send(packet)
    packet = struct.pack("<B", args[0])
    sock.send(packet)
    received = sock.recv(1)
    hdr = struct.unpack("<B", received)


def special_released(*args):
	# If escape is pressed, kill everything.
    print str(args[0]) + " released"
    global velmult
    global steermult
    global defvelocity
    global defsteerangle
    global steerangle
    global velocity



    global sock
    packet = struct.pack("<B", 2)
    sock.send(packet)
    packet = struct.pack("<B", args[0])
    sock.send(packet)
    received = sock.recv(1)
    hdr = struct.unpack("<B", received)


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
    	
