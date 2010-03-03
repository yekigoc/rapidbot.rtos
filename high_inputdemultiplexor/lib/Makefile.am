lib_LTLIBRARIES = libinputdemultiplexor.la
libinputdemultiplexor_la_SOURCES = rgi-inputdemultiplexor.c
include_HEADERS = rgi-inputdemultiplexor.h
libinputdemultiplexor_la_LDFLAGS = -Wl,--export-dynamic -rdynamic -lgobject-2.0 -lgio-2.0 -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -pthread -lgthread-2.0
libinputdemultiplexor_la_CFLAGS = -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0/gio

%.c %.h %-private.h: %.gob
	@GOB2@ $<
