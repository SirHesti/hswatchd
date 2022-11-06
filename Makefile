all: hswatchd $(MANP)

#-[ import_tools ]
.PHONY: clean install
TDIR := .
MANP := hswatchd.8

install: hswatchd $(MANP) hswatchd.rc
	service hswatchd stop
	cp hswatchd /usr/sbin
	[ -f /etc/hswatchd.rc ] || cp hswatchd.rc /etc
	service hswatchd start
	gzip -c -9 $(MANP) >/usr/share/man/man8/hswatchd.8.gz
	mandb >/dev/null 2>&1
#-[ ------------ ]

CPPFLAGS := -MMD -MP -O2 -Wall -pipe -D_FILE_OFFSET_BITS=64 -I$(TDIR)

hswatchd: hswatchd.o tools.o
	$(CXX) -s hswatchd.o tools.o -o hswatchd $(LDFLAGS)

hswatchd.o: hswatchd.c VERSION.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c hswatchd.c -o hswatchd.o

tools.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(TDIR)/tools.c -o tools.o

clean:
	rm -f hswatchd.o tools.o \
		  hswatchd.d tools.d \
		  hswatchd
		  
