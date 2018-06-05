DESCRIPTION = "IceWM window manager"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=4a26952467ef79a7efca4a9cf52d417b"

DEPENDS = "virtual/libx11 libxext libxcomposite libxfixes libxdamage libxrender \
           libxinerama libxpm xrandr libxft linuxdoc-tools-native gdk-pixbuf \
           "

SRC_URI = "file://icewm_1.3.12.tar.gz \
           file://echo_with_-e_not_working.patch \
           file://0001-Fix-refresh-window-but-still-allow-fullscreen-resize.patch \
           file://gen-preferences-in-postinst.patch \
           file://toolbar \
           file://keys \
           file://default.theme \
           file://startup \
           file://shutdown \
           "

inherit autotools pkgconfig gettext

S = "${WORKDIR}/icewm"

EXTRA_OECONF += "--enable-corefonts --enable-xfreetype --disable-debug \
                 --docdir=${docdir} --htmldir=${docdir}/html --prefix=${prefix} \
                 --datadir=${datadir} --disable-menus-gnome2"

do_configure_prepend() {
	sed -i "s/autoreconf -iv/autoreconf -fiv/g" ${S}/autogen.sh
	(cd ${S}; ./autogen.sh)
}


ROOT_HOME = "/root"

do_install_append() {
	# icewm config(s) file
	install -d ${D}${ROOT_HOME}/.icewm
	install -m 0755 ${WORKDIR}/startup		${D}${ROOT_HOME}/.icewm
	install -m 0755 ${WORKDIR}/shutdown		${D}${ROOT_HOME}/.icewm
	install -m 0644 ${WORKDIR}/toolbar		${D}${datadir}/icewm
	install -m 0644 ${WORKDIR}/keys			${D}${datadir}/icewm
	install -d ${D}${datadir}/icewm/themes/default
	install -m 0644 ${WORKDIR}/default.theme	${D}${datadir}/icewm/themes/default/

	# run xfsettingsd daemon at icewm startup if support controlpanel
	if ${@base_contains('DISTRO_FEATURES', 'ctrlpanel', 'true', 'false', d)}
	then
		echo "xfsettingsd" >> ${D}${ROOT_HOME}/.icewm/startup
	fi

	# run ModemManager and NetworkMnager in background if support LTE
	if ${@base_contains('DISTRO_FEATURES', 'lte', 'true', 'false', d)}
	then
		echo "NetworkManager &" >> ${D}${ROOT_HOME}/.icewm/startup
		echo "ModemManager &"   >> ${D}${ROOT_HOME}/.icewm/startup
	fi
}

FILES_${PN} += "${datadir}/xsessions/* \
                ${ROOT_HOME}/.icewm/* \
                "
RDEPENDS_${PN} += "libstdc++"
