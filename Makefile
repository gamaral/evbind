CFLAGS := -O2 -W -Wall
DESTDIR := /usr/local

all: evbind

evbind: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o evbind main.c

clean:
	rm -f evbind

install: evbind
	install -s evbind ${DESTDIR}/sbin/
	install -D default.sh /etc/evbind/default.sh
