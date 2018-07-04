SUMMARY = "ACS smart card middleware"
DESCRIPTION = "Acs middleware for smart card"

HOMEPAGE = "http://www.acs.com.hk"
SECTION = "Driver/Library"

DEPENDS = ""

LICENSE = "LGPLv2+"
LIC_FILES_CHKSUM = "file://usr/share/doc/libacsccid1/copyright;md5=12da94ccd421ce9eed5b117fd28004f7"

SRC_URI = "file://acs-smart-card-reader-driver-${PV}.tar.gz"
S = "${WORKDIR}/acs-smart-card-reader-driver-${PV}"
INSANE_SKIP_${PN} = "already-stripped"

do_install() {
	cp -r ${S}/* ${D}
}

FILES_${PN} += "\
    ${libdir}/* \
"
FILES_${PN}-doc += "\
    patches \
    ${docdir}/* \
"
