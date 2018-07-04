DESCRIPTION = "Support TZCS big core file"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

DEPENDS = "gtk+ tzcslib"

PR = "r0"

SRC_URI = " \
    file://Makefile \
    file://bios_update.c \
    file://check-sdcache.c \
    file://check-webauth.c \
    file://heatrun.c \
    file://service.c \
    file://tzcs-polling.c \
    file://disp-status.c \
    file://de.mo \
    file://ja.mo \
"

S = "${WORKDIR}"

do_compile() {
	oe_runmake
	${CC} disp-status.c -o disp-status `pkg-config --cflags --libs gtk+-2.0`
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${B}/service ${D}${bindir}
	install -m 0755 ${B}/check-webauth ${D}${bindir}
	install -m 0755 ${B}/check-sdcache ${D}${bindir}
	install -m 0755 ${B}/tzcs-polling ${D}${bindir}
	install -m 0755 ${B}/heatrun ${D}${bindir}
	install -m 0755 ${B}/disp-status ${D}${bindir}
	install -d ${D}/usr/share/locale/ja/LC_MESSAGES
	install -d ${D}/usr/share/locale/de/LC_MESSAGES
	install ${B}/ja.mo ${D}/usr/share/locale/ja/LC_MESSAGES/check-sdcache.mo
	install ${B}/de.mo ${D}/usr/share/locale/de/LC_MESSAGES/check-sdcache.mo

}

RDEPENDS_${PN} = "gtk+ ${PN}-locale-ja ${PN}-locale-de tzcslib"
