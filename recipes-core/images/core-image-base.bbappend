inherit image_meson
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
PACKAGE_INSTALL += "\
                   initramfs-meson-boot \
                   e2fsprogs \
                   mali-examples \
                   fbscripts \
                   directfb \
                   qtbase \
                   qt5everywheredemo \
                   qt5-opengles2-test \
                   vim \
                   rpm \
                   remotecfg \
                   gst-plugin-aml-asink \
                   gst-player \
                   playscripts \
                   ntfsprogs \
                   ntfs-3g \
                   "
