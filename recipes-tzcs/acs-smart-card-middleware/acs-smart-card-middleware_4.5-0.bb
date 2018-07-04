SUMMARY = "ACS smart card middleware"
DESCRIPTION = "Acs middleware for smart card"

HOMEPAGE = "http://www.acs.com.hk"
SECTION = "Driver/Library"

DEPENDS = ""

LICENSE = "LGPLv2+"
LIC_FILES_CHKSUM = "file://copyright;md5=12da94ccd421ce9eed5b117fd28004f7"


SRC_URI = "file://acs-smart-card-middleware-${PV}.tar.gz"
S = "${WORKDIR}/acs-smart-card-middleware-${PV}"
#INSANE_SKIP_${PN} = "already-stripped"
INSANE_SKIP_${PN} = "dev-so"
INSANE_SKIP_${PN}-lib = "dev-so"

PACKAGES =+ "${PN}-lib"
do_install() {
	cp -r ${S}/* ${D}
	rm ${D}/copyright
	rm -r ${D}${bindir}/Middleware/i386/ 
	rm -r ${D}/usr/local/bin/Middleware/i386/
	rm ${D}/usr/bin/Middleware/amd64/libacospkcs11.so.4.5.0
 	rm ${D}/usr/local/lib/*
	rm ${D}/usr/local/bin/Middleware/amd64/libacospkcs11.so.4.5.0
}
FILES_${PN}-dbg += "\
	${exec_prefix}/local/bin/.debug/* \
	${exec_prefix}/local/bin/*/.debug/* \
	${exec_prefix}/local/bin/*/*/.debug/* \
	${exec_prefix}/local/lib/.debug/* \
	${bindir}/*/.debug/* \
	${bindir}/*/*/.debug/* \
"
FILES_${PN} += "\
	${bindir}/* \
	${baselibdir}/* \
	${exec_prefix}/local/* \
	${exec_prefix}/local/.acscmu/* \
"
FILES_${PN}-doc += "\
	patches \
	${datadir}/* \
"
FILES_${PN}-lib += "\
	${libdir}/libacospkcs11.so.4.5.0 \
	${libdir}/vmware/* \
"
do_install_append(){
	mkdir -p ${D}${libdir}/vmware/view/pkcs11
	ln -s ${libdir}/libacospkcs11.so.4.5.0 ${D}${libdir}/vmware/view/pkcs11/libacospkcs11.so
}
