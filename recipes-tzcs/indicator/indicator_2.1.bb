DESCRIPTION = "Indicator application to display wifi signal, brightness level, \
sound volume, Touchpad status, battery power"
SECTION = "util"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://COPYING;md5=42de1be97416ffbc6de7169e66f11084"
PR = "r0"

SRC_URI = "file://*"

DEPENDS = "gtk+ tzcslib libtzcs-security udev"

RDEPENDS_${PN} = "gtk+ tzcslib libtzcs-security xdotool ethtool xinput libx11 \
libxrandr rfkill wpa-supplicant libudev"

S = "${WORKDIR}"

inherit autotools pkgconfig

EXTRA_OECONF += "${@bb.utils.contains("DISTRO_FEATURES", "ctrlpanel", "--enable-ctrlpanel", "", d)} \
                 ${@bb.utils.contains("DISTRO_FEATURES", "ctrlpanel", "--enable-switchmode", "", d)} \
                 ${@bb.utils.contains("DISTRO_FEATURES", "makefullscreen", "--enable-vdifullscreen", "", d)}"

do_install_append() {
    install -d ${D}${datadir}/${PN}
    cp -r ${S}/resource ${D}${datadir}/${PN}
    cp -r ${S}/glade ${D}${datadir}/${PN}
}

