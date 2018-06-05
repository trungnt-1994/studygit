DESCRIPTION = "OpenConnect is an SSL VPN client initially created to support \
               Cisco's AnyConnect SSL VPN"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING.LGPL;md5=243b725d71bb5df4a1e5920b344b86ad"

PR = "r0"

DEPENDS = "zlib openssl libxml2 vpnc-scripts"

inherit autotools pkgconfig

SRC_URI = "file://openconnect_7.06.orig.tar.gz"

S = "${WORKDIR}/${PN}-${PV}"

EXTRA_OECONF += "--with-vpnc-script=${STAGING_DATADIR}/vpnc-scripts/vpnc-script"

do_configure_prepend() {
	oe_runconf
}

RDEPENDS_${PN} = "vpnc-scripts openssl zlib libxml2"
