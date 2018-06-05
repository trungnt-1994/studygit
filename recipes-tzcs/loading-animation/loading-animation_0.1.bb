DESCRIPTION = "Loading annimation until firefox loading the page completely"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://loading.c;beginline=1;endline=10;md5=1551978876e482f9cd4df8cdc1cab9ea"

PR = "r0"

DEPENDS = "gtk+"
RDEPENDS_${PN} = "gtk+ libx11 libxrandr"

SRC_URI = "file://loading.c \
           file://loading.gif \
	   "
S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
    ${CC} loading.c -o loading -lX11 -lXrandr `pkg-config --cflags --libs gtk+-2.0`
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/loading ${D}${bindir}
    install -d ${D}${datadir}/${PN}
    install -m 0644 ${WORKDIR}/loading.gif ${D}${datadir}/${PN}
}
