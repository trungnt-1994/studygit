DESCRIPTION = "Restart require when certificate renewal is done on TMZC server before certificate is expired"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://restart_required.c;beginline=1;endline=10;md5=86ea8b25c620b518d1aef4afff661c5a"

PR = "r0"

DEPENDS = "gtk+"

SRC_URI = "file://restart_required.c \
           file://branch-check \
	   "
S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
    ${CC} restart_required.c -o restart-required `pkg-config --cflags --libs gtk+-2.0`
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/restart-required ${D}${bindir}
    install -m 0755 ${WORKDIR}/branch-check ${D}${bindir}
}
