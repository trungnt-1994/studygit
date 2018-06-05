DESCRIPTION = "TZCS Automation testing tool"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://COPYING;md5=a8faebde25fed601f57530a4c1a1bfde"

SRC_URI = " \
    file://COPYING \
    file://hotkeys-lf.tar.gz \
    file://vmware.tar.gz \
    file://indicator.tar.gz \
"

S = "${WORKDIR}"

do_configure[noexec] = "1"

do_compile[noexec] = "1"

do_install() {
	# support hotkeys-lf automation test
	cp -r ${B}/hotkeys-lf/* ${D}
	# support vmware automation test
	cp -r ${B}/vmware/* ${D}
	# support indicator automation test
	cp -r ${B}/indicator/* ${D}
}

INSANE_SKIP_${PN} = "already-stripped"
