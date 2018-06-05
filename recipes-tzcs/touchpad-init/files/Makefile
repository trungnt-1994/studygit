objects = touchpad_init.o 

CFLAGS ?= -g -O0

all: touchpad-init 

touchpad-init: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

clean:
	-rm touchpad_init >/dev/null 2>&1
	-rm *.o >/dev/null 2>&1

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 touchpad-init $(DESTDIR)$(bindir)
	install -m 0755 Synaptic_sst $(DESTDIR)$(bindir)
	install -d $(DESTDIR)$(sysconfdir)/init.d
	install -m 0755 touchpad-ini $(DESTDIR)$(sysconfdir)/init.d
	install -d $(DESTDIR)$(sysconfdir)
	install -m 0644 syntp_sensitive.conf $(DESTDIR)$(sysconfdir)

uninstall:
	-rm $(DESTDIR)$(bindir)/touchpad-init >/dev/null 2>&1
	-rm $(DESTDIR)$(bindir)/Synaptic_sst >/dev/null 2>&1
	-rm $(DESTDIR)$(sysconfdir)/init.d/touchpad-init >/dev/null 2>&1
	-rm $(DESTDIR)$(sysconfdir)/syntp_sensitive.conf >/dev/null 2>&1

.PHONY: all clean install uninstall
