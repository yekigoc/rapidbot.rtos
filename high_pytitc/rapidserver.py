import SocketServer
import struct
import time
import array
import sys
sys.path.append('/usr/local/lib/python2.6/dist-packages/usb/')
import usb.core
import usb.util
import threading

class rapidtcphandler(SocketServer.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # self.request is the TCP socket connected to the client
        while (True):
            try:
                self.data = self.request.recv(1)
            except socket.error:
                self.request.close()
                return
            if len(self.data) >= 1:
                hdr = struct.unpack("<B", self.data)
            else:
                print ("quitting")
                return
#            print ("header = " + str(hdr))
            
            if hdr[0] == 1:
            #set dutycycles
                self.data = self.request.recv(1)
                tmp = struct.unpack("<B", self.data)
                packet = struct.pack("B", 255)
                self.request.send(packet)
                if (tmp[0]==101):
        #forward
                    self.server.velmult=-1.0
                if (tmp[0]==103):
        #reverse
                    self.server.velmult=1.0
                if (tmp[0]==100):
        #left
                    self.server.steermult=-1.0
                if (tmp[0]==102):
        #right
                    self.server.steermult=1.0
            if hdr[0] == 2:
            #set dutycycles
                self.data = self.request.recv(1)
                tmp = struct.unpack("<B", self.data)
                packet = struct.pack("B", 255)
                self.request.send(packet)
                if (tmp[0]==101):
        #forward
                    self.server.velmult=0.0
                    self.server.velocity=self.server.defvelocity
                if (tmp[0]==103):
        #reverse
                    self.server.velmult=0.0
                    self.server.velocity=self.server.defvelocity
                if (tmp[0]==100):
        #left
                    self.server.steermult=0.0
                    self.server.steerangle=self.server.defsteerangle
                if (tmp[0]==102):
        #right
                    self.server.steermult=0.0
                    self.server.steerangle=self.server.defsteerangle



class rapidtcpserver(SocketServer.TCPServer):
    ""
    def __init__(self):
        self.velmult=0.0
        self.steermult=0.0
        self.steerspeed=1
        self.acceleration=0.1
        self.defvelocity=375.0
        self.defsteerangle=380.0        
        self.velocity=self.defvelocity
        self.steerangle=self.defsteerangle
        self.exitflag = 0
        self.allow_reuse_address = True

    def init(self, ui=None, port = 30000):

#        self.server = SocketServer.TCPServer(("", port), ElemTCPHandler)
        SocketServer.TCPServer.__init__(self,("", port), rapidtcphandler)
        print 'started rapid server...'
        self.ui=ui
        self.thread=threading.Thread(target=self.serve_forever)
        self.thread.setDaemon(True)
        self.thread.start()
#        print "Device server loop running in thread:", self.thread.getName()
        
    def shutdown(self):
        self.socket.close()


def main():
    try:
        server=rapidtcpserver()
        server.init()
        dev = usb.core.find(idVendor=0xEB03, idProduct=0x0920)

        # was it found?
        if dev is None:
            raise ValueError('Device not found')

        dev.set_configuration()

#        CTRL_OUT = [0x81, 0x0]
        dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, [0])

        while(True):
            server.velocity = server.velocity+server.velmult*server.acceleration
            server.steerangle = server.steerangle+server.steermult*server.steerspeed
            if (server.velocity >= 400):
                server.velocity = 400
            if (server.velocity <= 365):
                server.velocity = 365
            if(server.steerangle > 500):
                server.steerangle = 500
            if(server.steerangle < 293):
                server.steerangle = 293
                
            print ("angle = "+str(server.steerangle) + " velocity = " + str(server.velocity))
            #            vel = array.array("B", [int(server.velocity)])
            #            print (vel)
            vel = struct.pack("H", server.velocity)
            ang = struct.pack("H", server.steerangle)
            #            print ("vel = " + str(vel))
            #            print("len(vel)" + str(len(vel)))
            ret = dev.ctrl_transfer(0x01, 0x0, 0x05, 0, vel);
            ret = dev.ctrl_transfer(0x01, 0x0, 0x02, 0, ang);
            time.sleep(0.001)

    except KeyboardInterrupt:
        server.shutdown()

if __name__ == "__main__":
    main()
