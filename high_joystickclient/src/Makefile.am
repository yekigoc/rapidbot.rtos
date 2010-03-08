bin_PROGRAMS = joystickclient
joystickclient_SOURCES = main.c joystick.c ../../high_inputdemultiplexor/lib/rgi.pb-c.c
joystickclient_LDFLAGS = -Wl,--export-dynamic -rdynamic -lprotobuf-c -pthread -lm
joystickclient_CFLAGS = -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/google/protobuf-c -std=gnu99 -I../../high_inputdemultiplexor/lib -I/usr/include/libusb-1.0

