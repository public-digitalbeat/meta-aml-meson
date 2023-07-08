SUMMARY = "amlogic gstreamer app"
LICENSE = "LGPL-2.0+"
LIC_FILES_CHKSUM = "file://COPYING;md5=e431e272f5b8a6a4f948a910812f235e"
SRC_URI = "http://10.28.39.121:8088/testfile/gst-app-0.11.0.tar.gz"
SRC_URI[md5sum] = "3353a00b1656b4d9da1b35214c3ed3b7"
S = "${WORKDIR}/gst-app-0.11.0"
EXTRA_OEMAKE = "CROSS=${TARGET_PREFIX} TARGET_DIR=${STAGING_DIR_TARGET} STAGING_DIR=${D} DESTDIR=${D}"
inherit autotools pkgconfig features_check
#FILES_${PN} += "/usr/lib/gstreamer-1.0/*"
INSANE_SKIP_${PN} = "ldflags dev-so "
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1" 

