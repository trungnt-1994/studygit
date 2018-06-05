DESCRIPTION = "Driver to support touch screen for Altair LE +"

LICENSE = "LGPL-2.0"
LIC_FILES_CHKSUM = "file://COPYING;md5=3100ccf84e983ce04ebb1bbdb5c0f68b"

SRC_URI = "file://altair-le-touchdriver.tar.gz"

S = "${WORKDIR}/${PN}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
	cp -r  ${WORKDIR}/altair-le-touchdriver/* ${D}
	rm ${D}/COPYING
	rm -rf ${D}/patches
	rm -rf ${D}${datadir}/X11/xorg.conf.d/51-syntp-styk.conf
	rm -rf ${D}/opt/Synaptics/SynTP/Cpl/res
	rm -rf ${D}/opt/Synaptics/SynTP/Enh/res
	rm -rf ${D}/opt/Synaptics/SynTP/share/Images/*
	rm -rf ${D}/opt/Synaptics/SynTP/SynCntxt.rtf
	rm -rf ${D}/opt/Synaptics/SynTP/SynLogo.bmp
	rm -rf ${D}/opt/Synaptics/bin/SynTPCpl
	rm -rf ${D}/opt/Synaptics/samples
	sed -i '5,7 s/^/#/' ${D}${sysconfdir}/X11/Xsession.d/76synenhapp
}

FILES_${PN} =+ "/"
FILES_${PN}-dbg += "/opt/Synaptics/lib/.debug"

INSANE_SKIP_${PN} = "already-stripped"

pkg_postinst_${PN}() {
ln -sf libwx_gtk2u-2.8.so.0.5.0 $D/opt/Synaptics/lib/libwx_gtk2u-2.8.so.0
ln -sf libwx_gtk2u-2.8.so.0 $D/opt/Synaptics/lib/libwx_gtk2u-2.8.so
ln -sf /var/tmp/SynPD.inf $D

cd $D/opt/Synaptics/SynTP/Cpl
tar zxf synaptics_Cpl_langs.tar.gz
rm -f synaptics_Cpl_langs.tar.gz
rm $(ls -I liblangUS.so)

cd $D/opt/Synaptics/SynTP/Enh
tar zxf synaptics_Enh_langs.tar.gz
rm -f synaptics_Enh_langs.tar.gz
rm $(ls -I liblangUS.so)

mkdir -p $D/var/lib/Synaptics

#This part is just for Xandros touchpad control panel redirection
if [ -h $D${bindir}/tputility ]; then
	update-alternatives --install ${bindir}/tputility tputility \
	    $D/opt/Synaptics/bin/SynTPCpl 60
	update-alternatives --set tputility /opt/Synaptics/bin/SynTPCpl
fi

if [ -e $D${datadir}/applications/gsynaptics.desktop ]; then
	# gnome-help from SLED 11 SP1 didn't like the original filename "gsynaptics-foss"
	# used for backup, so renamed to what's below
	mv -f $D${datadir}/applications/gsynaptics.desktop $D${datadir}/applications/gsynaptics.desktop-foss
fi

#If fastinit doesn't exist then this isn't a Xandros system and we don't need synaptics-init.sh
if ! [ -x $D${base_sbindir}/fastinit ]; then
	rm -f $D${base_sbindir}/synaptics-init.sh
fi

cd $D/opt
chown -R root:root Synaptics
chown root:root $D${libdir}/xorg/modules/input/syntp_drv.so
chown root:root $D${datadir}/X11/xorg.conf.d/50-syntp.conf
chown root:root $D${base_libdir}/udev/rules.d/75-synset.rules
chown root:root $D${sysconfdir}/init.d/synaptics-dbus
chown root:root $D${sysconfdir}/X11/Xsession.d/76synenhapp
chmod 644 $D${libdir}/xorg/modules/input/syntp_drv.so
chmod 644 $D${datadir}/X11/xorg.conf.d/50-syntp.conf
chmod 644 $D${base_libdir}/udev/rules.d/75-synset.rules
chmod 755 $D${sysconfdir}/init.d/synaptics-dbus
}
