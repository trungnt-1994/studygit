DESCRIPTION = "Support TZCS policy library file"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

PR = "r0"

SRC_URI = "file://libpolicy.tar.gz"

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${libdir}
	install -m 0555 ${WORKDIR}/libpolicy/libpolicy.so.1.0.0 ${D}${libdir}
	ln -s libpolicy.so.1.0.0 ${D}${libdir}/libpolicy.so.1
	ln -s libpolicy.so.1 ${D}${libdir}/libpolicy.so
}

RDEPENDS_${PN} = "expat"
