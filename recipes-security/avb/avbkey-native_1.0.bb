DESCRIPTION = "Android Verified Boot 2.0 Test Keys"
LICENSE = "APACHE"
LIC_FILES_CHKSUM = "file://${COREBASE}/../meta-amlogic/license/APACHE;md5=b8228f2369d92593f53f0a0685ebd3c0"

SRC_URI += "file://testkey_rsa2048.pem"
SRC_URI += "file://testkey_rsa4096.avbpubkey"
SRC_URI += "file://testkey_rsa4096.pem"

inherit native
S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${sysconfdir}
    install -m 0755 ${S}/testkey_rsa2048.pem ${D}${sysconfdir}
    install -m 0755 ${S}/testkey_rsa4096.pem ${D}${sysconfdir}
    install -m 0755 ${S}/testkey_rsa4096.avbpubkey ${D}${sysconfdir}
}
