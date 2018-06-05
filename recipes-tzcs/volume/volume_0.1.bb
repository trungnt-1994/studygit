DESCRIPTION = "Handler for Fn + 3/4 key in Toshiba keyboard"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://volume_controller.c;beginline=1;endline=10;md5=09261f00c531120edc09b3f38496ad12 \
                    file://vol.h;beginline=1;endline=10;md5=5b27dd4619da4f6517841b959cbe59eb"

DEPENDS = "alsa-lib"

PR = "r0"

SRC_URI = "file://volume_controller.c \
           file://vol.h \
           "

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile() {
	${CC} volume_controller.c -o volume_set -lm -lasound
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/volume_set ${D}${bindir}
}
