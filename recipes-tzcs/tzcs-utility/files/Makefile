objects = write_proc_power.o

CFLAGS ?= -Wall -O2

all: write_proc_power

write_proc_power: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@ $(LDFLAGS)

clean:
	-rm eula >/dev/null 2>&1
	-rm *.o >/dev/null 2>&1

install:
	install -d $(DESTDIR)$(bindir)
	install -m 0755 write_proc_power $(DESTDIR)$(bindir)

.PHONY: all clean install
