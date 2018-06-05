DESCRIPTION = "Network Time protocol daemon and utility programs"

PR = "r2"

PV = "4.2.8p10+dfsg"

LICENSE = "NTP"
LIC_FILES_CHKSUM = "file://COPYRIGHT;md5=e877a1d567a6a58996d2b66e3e387003"

SRC_URI += "file://${PN}-${PV}.tar.gz \
            file://ntpcheckup.c \
            file://startup-ntp.sh \
            file://ntp.init \
            "

inherit autotools pkgconfig useradd

USERADD_PACKAGES = "${PN}"
NTP_USER_HOME ?= "/var/lib/ntp"
USERADD_PARAM_${PN} = "--system --home-dir ${NTP_USER_HOME} \
                       --no-create-home \
                       --shell /bin/false --user-group ntp"

# Configure follow debian/rules
EXTRA_OECONF += "--enable-all-clocks --enable-parse-clocks --enable-SHM \
                --disable-debugging --sysconfdir=/var/lib/ntp \
                --with-sntp=no \
                --with-lineeditlibs=edit \
                --without-ntpsnmpd \
                --disable-local-libopts \
                --enable-ntp-signd \
                --disable-dependency-tracking \
                --with-yielding-select=yes \
                lo_cv_test_autoopts=no \
"

PACKAGECONFIG ??= "openssl cap"
PACKAGECONFIG[openssl] = "--with-openssl-libdir=${STAGING_LIBDIR} \
                          --with-openssl-incdir=${STAGING_INCDIR} \
                          --with-crypto, \
                          --without-openssl --without-crypto, \
                          openssl"
PACKAGECONFIG[cap] = "--enable-linuxcaps,--disable-linuxcaps,libcap"
PACKAGECONFIG[readline] = "--with-lineeditlibs,--without-lineeditlibs,readline"

do_compile_append() {
	${CC} -o ${WORKDIR}/ntpcheckup ${WORKDIR}/ntpcheckup.c -ltzcs
}

do_install_append(){
	install -D -m 0755 ${B}/scripts/ntpsweep/ntpsweep ${D}${bindir}/ntpsweep

	[ ! -d ${D}${sysconfdir}/dhcp/dhclient-exit-hooks.d ] && \
		install -d ${D}${sysconfdir}/dhcp/dhclient-exit-hooks.d
	install -D -m 0644 ${S}/debian/ntp.dhcp ${D}${sysconfdir}/dhcp/dhclient-exit-hooks.d/ntp
	install -D -m 0644 ${S}/debian/ntpdate.dhcp ${D}${sysconfdir}/dhcp/dhclient-exit-hooks.d/ntpdate
	install -D -m 0755 ${S}/debian/ntpdate-debian ${D}${sbindir}/ntpdate-debian

	# Install files from ${S}/debian
	[ ! -d ${D}${sysconfdir}/cron.daily ] && install -d ${D}${sysconfdir}/cron.daily
	install -m 0755 ${S}/debian/ntp.cron.daily	${D}${sysconfdir}/cron.daily/ntp
	[ ! -d ${D}${sysconfdir}/default ] && install -d ${D}${sysconfdir}/default
	install -m 0644 ${S}/debian/ntp.default		${D}${sysconfdir}/default/ntp
	install -m 0644 ${S}/debian/ntpdate.default	${D}${sysconfdir}/default/ntpdate
	[ ! -d ${D}${sysconfdir}/init.d ] && install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/ntp.init		    ${D}${sysconfdir}/init.d/ntp
	[ ! -d ${D}${sysconfdir}/logcheck/ignore.d.server ] && \
			install -d ${D}${sysconfdir}/logcheck/ignore.d.server
	install -m 0644 ${S}/debian/ntpdate.logcheck.ignore.server \
			${D}${sysconfdir}/logcheck/ignore.d.server/ntpdate
	[ ! -d ${D}${sysconfdir}/network/if-up.d ] && \
			install -d ${D}${sysconfdir}/network/if-up.d
	install -m 0755 ${S}/debian/ntpdate.if-up	${D}${sysconfdir}/network/if-up.d/ntpdate

	# remove all default ntp servers
	sed -i '/^pool.*iburst$/d' ${S}/debian/ntp.conf
	# add comment marks as where to put TMZCS server
	sed -i '/pool:/a ##################################################' ${S}/debian/ntp.conf
	sed -i '/pool:/a ##################################################' ${S}/debian/ntp.conf
	sed -i '/pool:/a ##################################################' ${S}/debian/ntp.conf
	install -D -m 0644 ${S}/debian/ntp.conf ${D}${sysconfdir}/ntp.conf

	# install ntp utility
	install -m 0755 ${WORKDIR}/ntpcheckup ${D}${bindir}
	install -m 0755 ${WORKDIR}/startup-ntp.sh ${D}${bindir}

	rm -rf ${D}${libdir}
}

PACKAGES =+ "ntpdate"

FILES_ntpdate = " \
	${sysconfdir}/*/ntpdate \
	${sysconfdir}/*/*/ntpdate \
	${sbindir}/ntpdate* \
"
