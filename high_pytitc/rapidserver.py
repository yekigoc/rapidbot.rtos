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
    def recvpress(self):
#set dutycycles
        self.data = self.request.recv(1)
        tmp = struct.unpack("<B", self.data)
#        packet = struct.pack("B", 255)
#        self.request.send(packet)
        print tmp[0]
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
        if (tmp[0]==49):
            self.server.lights[0]=1
            self.server.lightsapply[0]=1
        if (tmp[0]==50):
            self.server.lights[1]=1
            self.server.lightsapply[1]=1
        if (tmp[0]==51):
            self.server.lights[2]=1
            self.server.lightsapply[2]=1


    def recvrelease(self):
#set dutycycles
        self.data = self.request.recv(1)
        tmp = struct.unpack("<B", self.data)
#        packet = struct.pack("B", 255)
#        self.request.send(packet)
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
        if (tmp[0]==49):
            self.server.lights[0]=0
            self.server.lightsapply[0]=1
        if (tmp[0]==50):
            self.server.lights[1]=0
            self.server.lightsapply[1]=1
        if (tmp[0]==51):
            self.server.lights[2]=0
            self.server.lightsapply[2]=1

    def sendtelemetry(self):
        i = 0
        packet = struct.pack("<B", 254)
        self.request.send(packet)
        while i<8:
            packet = struct.pack("I", self.server.telemetry[i])
            self.request.send(packet)
            i=i+1


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
                self.recvpress()
            if hdr[0] == 2:
                self.recvrelease()
            if hdr[0] == 3:
                self.sendtelemetry()


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
        self.lights=[0,0,0]
        self.lightsapply=[0,0,0]
        self.telemetry=[0,0,0,0,0,0,0,0]

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

#            print ("angle = "+str(server.steerangle) + " velocity = " + str(server.velocity))

            vel = struct.pack("H", server.velocity)
            ang = struct.pack("H", server.steerangle)

            ret = dev.ctrl_transfer(0x01, 0x0, 0x05, 0, vel);
            ret = dev.ctrl_transfer(0x01, 0x0, 0x02, 0, ang);

            i=0
            while i<len(server.lightsapply):
                if server.lightsapply[i] == 1:
                    server.lightsapply[i] = 0
                    led = struct.pack("BB", i, server.lights[i])
                    dev.ctrl_transfer(0x01, 0x0, 0x07, 0x0, led)
                i=i+1


#telemetry code
            adc = dev.ctrl_transfer(0x81, 0x0, 0x08, 0x0, 4*8)
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
            server.telemetry[0]=adcv1[0]
            server.telemetry[1]=adcv2[0]
            server.telemetry[2]=adcv3[0]
            server.telemetry[3]=adcv4[0]
            server.telemetry[4]=adcv5[0]
            server.telemetry[5]=adcv6[0]
            server.telemetry[6]=adcv7[0]
            server.telemetry[7]=adcv8[0]

            time.sleep(0.001)

    except KeyboardInterrupt:
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
        server.shutdown()

if __name__ == "__main__":
    main()
