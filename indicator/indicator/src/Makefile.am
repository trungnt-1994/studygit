if DEBUG
  AM_CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include -Wall -g -O3
else
  AM_CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include -Wall -O3
endif

AM_CFLAGS += $(GLIB_CFLAGS)

bin_PROGRAMS = indicator
indicator_SOURCES = indicator.c \
		    helper.c \
		    hdmi_certificate.c \
		    gui.c \
		    monitor.c \
		    network.c \
		    touchpad.c \
		    battery.c  \
		    brightness.c \
		    ctrlpanel.c

# Require core libs
indicator_CPPFLAGS = @CORE_CFLAGS@
indicator_LDADD = @CORE_LIBS@

CLEANFILES = *~
