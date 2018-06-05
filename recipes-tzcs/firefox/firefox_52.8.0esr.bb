DESCRIPTION = "Firefox browser"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRC_URI = "\
    file://firefox-52.8.0esr.tar.bz2 \
    ${@base_contains('DISTRO_FEATURES','suspend','file://firefox-profile-for-suspend.tar.gz','file://firefox-profile.tar.gz',d)} \
    file://linux_sslvpn.tgz \
    file://mime.types \
"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

ROOT_HOME ?= "/root"

install_sslvpn() {
	cp -r ${WORKDIR}${exec_prefix} ${D}
	chmod u+s ${D}${exec_prefix}/local/lib/F5Networks/SSLVPN/svpn_x86_64
}

do_install() {
	install -d ${D}/opt
	cp -r ${WORKDIR}/firefox ${D}/opt
	install -d ${D}${ROOT_HOME}
	cp -r ${WORKDIR}/firefox-profile ${D}${ROOT_HOME}/.mozilla
	install -m 0644 ${WORKDIR}/mime.types ${D}${ROOT_HOME}/.mime.types

	# install linux sslvpn
	install_sslvpn
}

PACKAGES = "${PN}"
FILES_${PN} = "/*"

INSANE_SKIP_${PN} = "already-stripped ldflags"

RDEPENDS_${PN} = "gtk+3"

pkg_postinst_${PN}() {
[ -e $D/opt/firefox/firefox ] && ln -s /opt/firefox/firefox $D${bindir}/firefox

[ -e $D${ROOT_HOME}/.mime.types ] && ln -s /opt/firefox/firefox $D${bindir}/gedit

if [ -e $D${exec_prefix}/local/lib/F5Networks/f5fpc_x86_64 ]; then
	install -d $D${exec_prefix}/local/bin
	ln -s ../../lib/F5Networks/f5fpc_x86_64 $D${exec_prefix}/local/bin/f5fpc
fi

touch -t "201511261024.38" $D${sysconfdir}/sv-pc
}
