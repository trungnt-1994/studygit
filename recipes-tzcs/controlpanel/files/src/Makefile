PROG = TzcsControlPanelMain
CC = gcc
SOURCES = TzcsControlPanelMain.c \
          TzcsCPTimeZone.c \
          TzcsAbout.c \
          TzcsDisplay.c \
          TzcsKeyboard.c \
          TzcsWidget.c \
          TzcsTouchpad.c \
          TzcsEULA.c

OBJS = $(SOURCES:.c=.o)
TZCS_CFLAGS = `pkg-config --cflags gtk+-2.0` -Wall -O2 -std=c99 -I${STAGING_INCDIR}/xfce4/xfconf-0
LDLIBS = `pkg-config --libs gtk+-2.0` -lX11 -ltzcs -lxfce4display -lXrandr -lxfconf-0

all : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(TZCS_CFLAGS) $(CPPFLAGS) $(LDLIBS)

.c.o :
	$(CC) $(TZCS_CFLAGS) $(CPPFLAGS) $(CFLAGS) -c $<

clean :
	rm -f $(PROG) $(OBJS)
