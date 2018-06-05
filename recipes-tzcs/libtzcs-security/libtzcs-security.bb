DESCRIPTION = "Support TZCS Security library file"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI = " \
    file://libtzcs-security.so.1.0.0 \
    file://libtzcs-security.h \
"

S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	install -d ${D}${libdir}
	install -m 0555 ${WORKDIR}/libtzcs-security.so.1.0.0 ${D}${libdir}
	ln -snf libtzcs-security.so.1.0.0 ${D}${libdir}/libtzcs-security.so.1
	ln -snf libtzcs-security.so.1 ${D}${libdir}/libtzcs-security.so

	install -d ${D}${includedir}/libtzcs-security
	install -m 0644 ${WORKDIR}/libtzcs-security.h ${D}${includedir}/libtzcs-security
}
