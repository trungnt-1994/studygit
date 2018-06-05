DESCRIPTION = "Enable/disable touchpad and Accure point"
SECTION = "util"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://syndaemon.c;beginline=1;endline=10;md5=ab11723e9196b5938d0b0691342e3be4"
PR = "r0"

SRC_URI = "file://syndaemon.c \
           file://syndaemon_start.c \
           file://test_hci.c \
           file://Touchpad_onoff.c \
           file://Touchpad_ctrl.c \
           file://tvalzctl.h"

DEPENDS = "gtk+ xdotool ethtool xinput libx11 libxrandr"
S = "${WORKDIR}"

inherit pkgconfig

do_compile() {
	cd ${WORKDIR}
	${CC} -g syndaemon.c -o syndaemon_tos -lpthread -lX11 -lXrandr `pkg-config --cflags --libs gtk+-2.0`
	${CC} -g syndaemon_start.c -o syndaemon_start -lpthread -lX11 -lXrandr `pkg-config --cflags --libs gtk+-2.0`
	${CC} -g test_hci.c -o test_hci -lpthread -lX11 -lXrandr `pkg-config --cflags --libs gtk+-2.0`
	${CC} -g Touchpad_onoff.c -o Touchpad_onoff -lpthread -lX11 -lXrandr `pkg-config --cflags --libs gtk+-2.0`
	${CC} -g Touchpad_ctrl.c -o Touchpad_ctrl -lpthread -lX11 -lXrandr `pkg-config --cflags --libs gtk+-2.0`
}
do_install() {
	install -d ${D}${bindir}
	install ${WORKDIR}/syndaemon_tos ${D}${bindir}
	install ${WORKDIR}/syndaemon_start ${D}${bindir}
	install ${WORKDIR}/test_hci ${D}${bindir}
	install ${WORKDIR}/Touchpad_onoff ${D}${bindir}
	install ${WORKDIR}/Touchpad_ctrl ${D}${bindir}
}
