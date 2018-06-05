DESCRIPTION = "Gtk popup displays message to user"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://gtkpopup.c;md5=c1aca9d1c97312e893e2a78327bb3a71"

DEPENDS = "gtk+"

PR = "r0"

SRC_URI = "file://gtkpopup.c \
           file://url.h"

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
	${CC} `pkg-config --cflags --libs gtk+-2.0` -o gtkpopup gtkpopup.c
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${B}/gtkpopup ${D}${bindir}
}
