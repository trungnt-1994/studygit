DESCRIPTION = "Support TZCS policy"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

PR = "r0"

SRC_URI = "file://policy.tar.gz"

S = "${WORKDIR}"

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/policy/GetClientStatus ${D}${bindir}
	install -m 0755 ${WORKDIR}/policy/GetClient ${D}${bindir}
	install -m 0755 ${WORKDIR}/policy/GetClientAll ${D}${bindir}
}

INSANE_SKIP_${PN} = "already-stripped"

RDEPENDS_${PN} = "expat libpolicy"
