DESCRIPTION = "Support GUI ping file display"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = " \
    file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28\
"

DEPENDS = "gtk+"
PR = "r0"

SRC_URI = "file://png-display.c \
           file://E038.png \
           file://E039.png \
           file://LostServer.png \
           file://LostServer_D.png \
           file://LostServer_J.png \
           file://PowerOFFfromTSCM.png \
           file://PowerOFFfromTSCM_D.png \
           file://PowerOFFfromTSCM_J.png \
           file://ScreenLock.png \
           file://ScreenLock_D.png \
           file://ScreenLock_J.png \
           "

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
    ${CC} png-display.c -o png-display -lX11 -lXrandr `pkg-config --cflags --libs gtk+-2.0`
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/png-display ${D}${bindir}
    install -d ${D}${datadir}/pngdisplay
    install -m 0644 ${WORKDIR}/*.png ${D}${datadir}/pngdisplay/
}

FILES_${PN} += "${datadir}/pngdisplay/*"
