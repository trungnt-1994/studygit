DESCRIPTION = "xrandr is used to set the size, orientation and/or \
               reflection of the outputs for a screen. It can also set \
               the screen size."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=fe1608bdb33cf8c62a4438f7d34679b3"

PR = "r0"

DEPENDS = "util-macros-native virtual/libx11 libxrender libx11 libxrandr"

XORG_PN = "${PN}"

SRC_URI = "file://xrandr_support_profile_v1.3.0_20150821.tar.gz"

inherit autotools_stage pkgconfig

S = "${WORKDIR}/xrandr"

FILES_${PN} += "${libdir}/X11/${XORG_PN}"

BBCLASSEXTEND = "native"
