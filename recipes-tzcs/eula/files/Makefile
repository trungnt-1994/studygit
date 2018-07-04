objects = eula.o

CFLAGS ?= -g -O2
LDFLAGS = $(shell pkg-config --libs gtk+-2.0 poppler-glib)

all: eula

eula: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@ $(LDFLAGS)

eula.o: CFLAGS := $(CFLAGS) $(shell pkg-config --cflags gtk+-2.0 poppler-glib)
eula.o: eula.c
	$(CC) $(CFLAGS) -c -o $@ eula.c

clean:
	-rm eula >/dev/null 2>&1
	-rm *.o >/dev/null 2>&1

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 eula $(DESTDIR)$(bindir)
	install -d $(DESTDIR)$(docdir)
	install -m 0644 License-$(VDIENV).pdf $(DESTDIR)$(docdir)/License.pdf

uninstall:
	-rm $(DESTDIR)$(bindir)/eula >/dev/null 2>&1

.PHONY: all clean install uninstall
