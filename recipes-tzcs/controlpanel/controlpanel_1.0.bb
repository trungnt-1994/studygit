DESCRIPTION = "Control Panel to EULA, Timezone, Display, Keyboard Settings, About"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://src/TzcsControlPanelMain.c;beginline=1;endline=2;md5=a6a03c100c94329b0ecd5964aa6be70d"

PR = "r0"

DEPENDS = "gtk+ tzcslib xrandr xfce4-settings"

SRC_URI = "file://src/* \
           file://resource/* \
           "

S = "${WORKDIR}"

inherit pkgconfig

do_configure[noexec] = "1"

do_compile() {
	cd ${S}/src
	oe_runmake STAGING_INCDIR="${STAGING_INCDIR}"
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${B}/src/TzcsControlPanelMain ${D}${bindir}/TzcsControlPanelMain
	install -m 0755 ${B}/src/ExecDisplay ${D}${bindir}/ExecDisplay
	install -m 0755 ${B}/src/ExecEula ${D}${bindir}/ExecEula

	install -d ${D}${datadir}/${PN}
	cp -r ${B}/resource ${D}${datadir}/${PN}

	install -d ${D}${datadir}/locale/ja/LC_MESSAGES
	install -d ${D}${datadir}/locale/de/LC_MESSAGES
	install -m 0644 ${B}/src/ja.mo ${D}${datadir}/locale/ja/LC_MESSAGES/tzcs-control-panel.mo
	install -m 0644 ${B}/src/de.mo ${D}${datadir}/locale/de/LC_MESSAGES/tzcs-control-panel.mo
}

RDEPENDS_${PN} = "setxkbmap gtk+ ${PN}-locale-ja ${PN}-locale-de"
