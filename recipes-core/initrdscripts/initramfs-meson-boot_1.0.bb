SUMMARY = "Meson init script"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"
DEPENDS = "virtual/kernel"
RDEPENDS_${PN} = "udev udev-extraconf"
SRC_URI = "file://init-meson.sh"

PR = "r0"

S = "${WORKDIR}"

do_install() {
        install -m 0755 ${WORKDIR}/init-meson.sh ${D}/init
        if [ "${@bb.utils.contains('DISTRO_FEATURES','READONLY','true','false',d)}" ];then
            touch ${D}/read-only
        fi
        install -d ${D}/dev
        mknod -m 622 ${D}/dev/console c 5 1
}

FILES_${PN} += " /init /dev "
FILES_${PN} += "${@bb.utils.contains('DISTRO_FEATURES', 'READONLY', ' /read-only', '', d)}"

# Due to kernel dependency
PACKAGE_ARCH = "${MACHINE_ARCH}"
