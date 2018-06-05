DESCRIPTION = "tzcs utilities"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://COPYING;md5=42de1be97416ffbc6de7169e66f11084"

SRC_URI = "\
    file://Makefile \
    file://write_proc_power.c \
    file://COPYING \
"

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
	unset CFLAGS
	unset LDFLAGS
	oe_runmake
}

do_install() {
	oe_runmake DESTDIR=${D} install
}
