DESCRIPTION = "Touchpad initialize"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://tvalzctl.h;md5=ad0ad101862940003700598573b8cbc4"

SRC_URI = " \
    file://touchpad-ini \
    file://touchpad_init.c \
    file://tvalzctl.h \
    file://Synaptic_sst \
    file://syntp_sensitive.conf \
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
