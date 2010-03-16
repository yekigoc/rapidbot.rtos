TGT=simple
#TGT=gtk
CC=gcc
DELETE=rm
ECHO=echo -e
CFLAGS+=-I/usr/include -fPIC -I/usr/include/libusb-1.0/ -std=gnu99
LDFLAGS+=-L/usr/lib -lusb-1.0

ifeq ($(TGT), gtk)
CFLAGS+=`pkg-config --cflags gtk+-2.0`
LDFLAGS+=`pkg-config --libs gtk+-2.0`
SOURCES=gtksteer.c
else
SOURCES=dpfp.c
endif
SHARED=
#SOURCES=gtksteer.c
#dpfp.c
#
OBJECTS=$(SOURCES:.c=.o)
BIN=gtksteer

BINDIR=./

all: $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(OBJECTS)
	$(ECHO) "----- Building simple evolution -----"
	$(CC) $(SHARED) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(DELETE) $(OBJECTS) $(BINDIR)/$(BIN)

