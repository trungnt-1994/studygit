DESCRIPTION = "Support battery alarm display"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

PR = "r0"

DEPENDS = "gtk+"

SRC_URI = "file://batteryalarm.c \
           file://tvalzctl.h \
           "

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
	${CC} batteryalarm.c -o batteryalarm `pkg-config --cflags --libs gtk+-2.0`
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${B}/batteryalarm ${D}${bindir}
}
