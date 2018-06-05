DESCRIPTION = "End user license agreement"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://tvalzctl.h;beginline=1;endline=22;md5=3c07aa14596371b4a7bd1558dca9c7e6"

DEPENDS = "poppler gtk+"

VDIENV = "${@bb.utils.contains('DISTRO_FEATURES', 'citrix', 'citrix', 'vmware',d)}"

SRC_URI = "\
    file://License-${VDIENV}.pdf \
    file://eula.c \
    file://tvalzctl.h \
    file://Makefile \
"

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
	unset CFLAGS
	unset LDFLAGS
	oe_runmake
}

do_install() {
	oe_runmake VDIENV=${VDIENV} DESTDIR=${D} install
}

RDEPENDS_${PN} = "eula-doc gtk+ libpoppler-glib"
