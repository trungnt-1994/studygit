SUMMARY = "Smart card library and applications"
DESCRIPTION = "OpenSC is a tool for accessing smart card devices. Basic\
functionality (e.g. SELECT FILE, READ BINARY) should work on any ISO\
7816-4 compatible smart card. Encryption and decryption using private\
keys on the smart card is possible with PKCS\
such as the FINEID (Finnish Electronic IDentity) card. Swedish Posten\
eID cards have also been confirmed to work."

HOMEPAGE = "http://www.opensc-project.org/opensc/"
SECTION = "System Environment/Libraries"
DEBIAN_PATCH_TYPE = "quilt"


DEPENDS = "pcsc-lite virtual/libiconv openssl"

LICENSE = "LGPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=7fbc338309ac38fefcd64b04bb903e34"

inherit autotools pkgconfig debian-package 

PV = "0.14.0"
DEBIAN_UNPACK_DIR = "${S}"
SRC_URI = "file://${BPN}-${PV}.tar.gz"
S = "${WORKDIR}/${BPN}-${PV}"

do_install_append() {
	mkdir -p ${D}${libdir}/vmware/view/pkcs11/
	ln -s ${libdir}/opensc-pkcs11.so ${D}${libdir}/vmware/view/pkcs11/libopensc-pkcs11.so	
}

RDEPENDS_${PN} = "readline"
PACKAGES =+ "${PN}-pkcs11"
INSANE_SKIP_${PN}-pkcs11 = "dev-so"

FILES_${PN} += "\
    ${sysconfdir}/* \
    ${bindir}/* \
    ${docdir}/* \
    ${datadir}/opensc/* \ 
"
FILES_${PN}-dev += "\
    ${libdir}/libopensc.so \
    ${libdir}/libsmm-local.so \
    ${libdir}/pkcs11/opensc-pkcs11.so \
    ${libdir}/pkcs11/onepin-opensc-pkcs11.so \
    ${libdir}/pkcs11/pkcs11-spy.so \
"
FILES_${PN}-pkcs11 += "\
    ${libdir}/libopensc.so.3 \
    ${libdir}/libopensc.so.3.0.0 \
    ${libdir}/libsmm-local.so.3 \
    ${libdir}/libsmm-local.so.3.0.0 \
    ${libdir}/onepin-opensc-pkcs11.so \
    ${libdir}/opensc-pkcs11.so \
    ${libdir}/pkcs11-spy.so \
    ${libdir}/vmware/* \
"
