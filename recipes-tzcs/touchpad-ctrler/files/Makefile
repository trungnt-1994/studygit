objects = linux_alps.o 

CFLAGS ?= -g -O0

all: psmouse_tos

psmouse_tos: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

clean:
	-rm psmouse_tos >/dev/null 2>&1
	-rm *.o >/dev/null 2>&1

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 psmouse_tos $(DESTDIR)$(bindir)

uninstall:
	-rm $(DESTDIR)$(bindir)/psmouse_tos >/dev/null 2>&1

.PHONY: all clean install uninstall
