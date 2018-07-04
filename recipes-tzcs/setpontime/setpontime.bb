DESCRIPTION = "Support T-Reboot time setting"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

DEPENDS = "tzcslib"

PR = "r0"

SRC_URI = "file://setpontime.tar.gz"

S = "${WORKDIR}/${PN}"

do_configure[noexec] = "1"

do_compile() {
	${CC} setpontime.c -o setpontime -ltzcs
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${B}/setpontime ${D}${bindir}
}

RDEPENDS_${PN} = "tzcslib"

