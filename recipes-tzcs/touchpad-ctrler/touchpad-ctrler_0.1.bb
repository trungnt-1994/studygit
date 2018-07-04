DESCRIPTION = "Enable/disable touchpad and Accure point"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://linux-alps.c;beginline=1;endline=10;md5=b0d0c039dc8d54c69c5d7a811f706b1a"

SRC_URI = "file://linux_alps.c \
           file://Makefile \
           "

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake DESTDIR=${D} install
}
