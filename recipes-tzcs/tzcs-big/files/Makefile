#
# Makefile
#
# Makefile for fbv

#include Make.conf

CC	= gcc 

LIB	= -ltzcs
#LIB	=

SOURCES1	= service.c bios_update.c
SOURCES2	= check-sdcache.c
SOURCES3	= check-webauth.c
SOURCES4	= heatrun.c
SOURCES5	= tzcs-polling.c

OBJECTS1	= ${SOURCES1:.c=.o}
OBJECTS2	= ${SOURCES2:.c=.o}
OBJECTS3	= ${SOURCES3:.c=.o}
OBJECTS4	= ${SOURCES4:.c=.o}
OBJECTS5	= ${SOURCES5:.c=.o}

OUT1	= service
OUT2	= check-sdcache
OUT3	= check-webauth
OUT4	= heatrun
OUT5	= tzcs-polling

#all: $(OUT1)
all:  $(OUT1) $(OUT2) $(OUT3) $(OUT4) $(OUT5)
	@echo Build DONE.

$(OUT1): $(OBJECTS1)
	$(CC) -o $(OUT1) -Wall -O2 $(SOURCES1) $(LIB)
$(OUT2): $(OBJECTS2)
	$(CC) -o $(OUT2) -Wall -O2 $(SOURCES2) $(LIB)
$(OUT3): $(OBJECTS3)
	$(CC) -o $(OUT3) -Wall -O2 $(SOURCES3) $(LIB)
$(OUT4): $(OBJECTS4)
	$(CC) -o $(OUT4) -Wall -O2 $(SOURCES4) $(LIB)
$(OUT5): $(OBJECTS5)
	$(CC) -o $(OUT5) -Wall -O2 $(SOURCES5) $(LIB)


clean:
	rm -f $(OBJECTS1) *~ $$$$~* *.bak $(OUT1)
	rm -f $(OBJECTS2) *~ $$$$~* *.bak $(OUT2)
	rm -f $(OBJECTS3) *~ $$$$~* *.bak $(OUT3)
	rm -f $(OBJECTS4) *~ $$$$~* *.bak $(OUT4)
	rm -f $(OBJECTS5) *~ $$$$~* *.bak $(OUT5)

##distclean: clean
##	@echo -e "error:\n\t@echo Please run ./configure first..." >Make.conf
##	rm -f $(OUT) config.h

#install: $(OUT1) $(OUT2) $(OUT3) $(OUT4) $(OUT5)
#	cp $(OUT1) $(DESTDIR)$(bindir)
#	cp $(OUT2) $(DESTDIR)$(bindir)
#	cp $(OUT3) $(DESTDIR)$(bindir)
#	cp $(OUT4) $(DESTDIR)$(bindir)
#	cp $(OUT5) $(DESTDIR)$(bindir)
#	gzip -9c $(OUT).1 > $(DESTDIR)$(mandir)/man1/$(OUT).1.gz

#uninstall: $(DESTDIR)$(bindir)/$(OUT)
#	rm -f $(DESTDIR)$(bindir)/$(OUT)
#	rm -f $(DESTDIR)$(mandir)/man1/$(OUT).1.gz

