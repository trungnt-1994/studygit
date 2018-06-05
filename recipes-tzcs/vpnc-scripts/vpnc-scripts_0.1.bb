DESCRIPTION = "This package contains scripts required to configure routing and \
               name services when invoked by the VPNC or OpenConnect Cisco VPN clients"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

PR = "r0"

SRC_URI = "file://vpnc-scripts_0.1~git20140806.orig.tar.gz"

S = "${WORKDIR}/${PN}-20140806"

do_configure[noexec] = "1"

do_compile[noexec] = "1"

do_install () {
	install -d ${D}${datadir}/vpnc-scripts
	install -m 0755 ${S}/vpnc-script ${D}${datadir}/vpnc-scripts
	install -m 0755 ${S}/vpnc-script-sshd ${D}${datadir}/vpnc-scripts
	install -m 0755 ${S}/vpnc-script-ptrtd ${D}${datadir}/vpnc-scripts
}
