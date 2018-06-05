DESCRIPTION = "Handler for F key in Toshiba keyboard"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://fhandler.c;beginline=1;endline=10;md5=d2342cc792cd24d84bb3d2c6de922731"

PR = "r0"

SRC_URI = "file://fhandler.c"

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
	${CC} -o fhandler fhandler.c
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/fhandler ${D}${bindir}
}

RDEPENDS_${PN} = "xinput"
