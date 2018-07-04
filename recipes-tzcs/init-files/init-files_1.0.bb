DESCRIPTION = "core scripts, configs and data for VDI environment"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

SRC_URI += "file://init.d/amixer-sset \
            file://init.d/mountdevsubfs \
            file://init.d/linuxreggen \
            file://init.d/alsa-set-def-card \
            file://init.d/locale-gen \
            file://init.d/pulseaudio-sysmod \
            file://init.d/save-power \
            file://init.d/chmod-selecttp \
            file://init.d/dev_install.sh \
            file://init.d/pcscd \
            file://init.d/ntpcheckup \
            "

install_initscripts() {
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/amixer-sset           ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/mountdevsubfs         ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/linuxreggen           ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/alsa-set-def-card     ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/locale-gen            ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/pulseaudio-sysmod     ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/save-power            ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/chmod-selecttp        ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/dev_install.sh        ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/pcscd                 ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/init.d/ntpcheckup            ${D}${sysconfdir}/init.d
}

ln_initscripts() {
	install -d ${D}${sysconfdir}/rcS.d
	ln -s ../init.d/udev                    ${D}${sysconfdir}/rcS.d/S04udev
	ln -s ../init.d/mountdevsubfs           ${D}${sysconfdir}/rcS.d/S05mountdevsubfs
	ln -s ../init.d/dev_install.sh          ${D}${sysconfdir}/rcS.d/S05dev-install
	ln -s ../init.d/ntpcheckup              ${D}${sysconfdir}/rcS.d/S07ntpcheckup
	ln -s ../init.d/dbus                    ${D}${sysconfdir}/rcS.d/S10dbus
	ln -s ../init.d/synaptics-dbus          ${D}${sysconfdir}/rcS.d/S16synaptics-dbus
	ln -s ../init.d/linuxreggen             ${D}${sysconfdir}/rcS.d/S17linuxreggen
	ln -s ../init.d/chmod-selecttp          ${D}${sysconfdir}/rcS.d/S18chmod-selecttp
	ln -s ../init.d/touchpad-ini            ${D}${sysconfdir}/rcS.d/S40touchpad-ini
	ln -s ../init.d/pulseaudio-sysmod       ${D}${sysconfdir}/rcS.d/S45pulseaudio-sysmod
	ln -s ../init.d/alsa-set-def-card       ${D}${sysconfdir}/rcS.d/S46alsa-set-def-card
	ln -s ../init.d/amixer-sset             ${D}${sysconfdir}/rcS.d/S47amixer-sset
	ln -s ../init.d/locale-gen              ${D}${sysconfdir}/rcS.d/S48locale-gen
	ln -s ../init.d/save-power              ${D}${sysconfdir}/rcS.d/S49save-power
	ln -s ../init.d/pcscd                   ${D}${sysconfdir}/rcS.d/S50pcscd

	install -d ${D}${sysconfdir}/rcE.d
	ln -s ../init.d/hwclock.sh              ${D}${sysconfdir}/rcE.d/K06hwclock.sh

	if ${@base_contains('DISTRO_FEATURES', 'vmware', 'true', 'false', d)}; then
	ln -s ../init.d/vmware-USBArbitrator    ${D}${sysconfdir}/rcS.d/S90vmware-USBArbitrator
	fi
}

ln_lib() {
	ln -s lib    ${D}/lib64
}

do_install() {
	install_initscripts
	ln_initscripts
	ln_lib
}

PACKAGES = "${PN}"
FILES_${PN} += "/*"
