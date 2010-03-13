lib_LTLIBRARIES = libinputdemultiplexor.la
libinputdemultiplexor_la_SOURCES = rgi-stateparam.c rgi-pwmparam.c rgi-adcparam.c rgi-timeparam.c rgi.pb-c.c rgi-usbscheduler.c rgi-inputdemultiplexor.c
include_HEADERS = rgi-inputdemultiplexor.h rgi-usbscheduler.h rgi-pwmparam.h rgi-stateparam.h rgi-adcparam.h rgi-timeparam.h
libinputdemultiplexor_la_LDFLAGS = -Wl,--export-dynamic -rdynamic -lgobject-2.0 -lgio-2.0 -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -pthread -lgthread-2.0 -lprotobuf-c -lusb-1.0
libinputdemultiplexor_la_CFLAGS = -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0/gio -I/usr/include/google/protobuf-c -std=gnu99 -I/usr/include/libusb-1.0 -I../../low_usb-controller/RTOSDemo/

%.c %.h %-private.h: %.gob
	@GOB2@ $<
