require conf/machine/include/arm/arch-armv8a.inc
include conf/machine/include/amlogic_externalsrc.inc
include conf/machine/include/amlogic_patches.inc
MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS += " \
        media-modules \
        optee-linux-driver \
        gpu \
        "
MACHINE_EXTRA_RRECOMMENDS = " kernel-modules kernel-devicetree"
EXTRA_IMAGEDEPENDS += "u-boot"
IMAGE_FSTYPES += "tar.gz ext4"
IMAGE_FSTYPES_DEBUGFS = ""
SERIAL_CONSOLES = "115200;ttyS0"
SERIAL_CONSOLES_t5w = "115200;ttyS0"
SERIAL_CONSOLES_sc2 = "921600;ttyS0"
SERIAL_CONSOLES_t5d = "921600;ttyS0"
SERIAL_CONSOLES_s4 = "921600;ttyS0"
SERIAL_CONSOLES_t7 = "921600;ttyS0"
PREFERRED_PROVIDER_virtual/kernel ?= "linux-meson"
KERNEL_IMAGETYPE_aarch64 = "Image.gz"
KERNEL_IMAGETYPE_armv7a = "uImage"
UBOOT_SUFFIX = "bin"
MACHINE_FEATURES = "usbgadget usbhost vfat "
IMAGE_BOOT_FILES ?= "u-boot.${UBOOT_SUFFIX}"
INITRAMFS_FSTYPES = "cpio.gz"
KERNEL_MODULE_AUTOLOAD += "mali"
TDK_VERSION = "TBD"
TDK_VERSION_g12a = "v3.8"
TDK_VERSION_sc2 = "v3.8/dev/S905X4"
TDK_VERSION_s4 = "v3.8/dev/S905Y4"
TDK_VERSION_t7 = "v3.8/dev/A311D2"
TDK_VERSION_t5w = "v3.8"

#AML_GIT_ROOT ?= "git@openlinux.amlogic.com/yocto"
#AML_GIT_PROTOCOL ?= "ssh"
#AML_GIT_ROOT ?= "code.rdkcentral.com/r/collaboration/soc/amlogic"
AML_GIT_ROOT ?= "github.com/public-digitalbeat"
AML_GIT_PROTOCOL ?= "https"

AML_GIT_ROOT_YOCTO_SUFFIX ?= ""
AML_META_LAYER ?= "meta-amlogic"
AML_GIT_ROOT_OP = "git@openlinux.amlogic.com"
AML_GIT_ROOT_PROTOCOL = "ssh"

MEDIA_MODULES_UCODE_BIN = "TBD"
MEDIA_MODULES_UCODE_BIN_t5d = "firmware/video_ucode.bin"
MEDIA_MODULES_UCODE_BIN_t5w = "firmware/video_ucode.bin"
MEDIA_MODULES_UCODE_BIN_s4 = "firmware/S905Y4/video_ucode.bin"
MEDIA_MODULES_UCODE_BIN_sc2 = "firmware/S905X4/video_ucode.bin"
MEDIA_MODULES_UCODE_BIN_t7 = "firmware/A311D2/video_ucode.bin"


#check connectivity using bing
#CONNECTIVITY_CHECK_URIS = "https://www.bing.com/"

LINUXLIBCVERSION ?= "4.9%"
PREFERRED_VERSION_libdrm = "2.4.100"
PREFERRED_VERSION_u-boot = "v2015.%"
WIDEVINE_VERSION = "16"

#for vulkan
PREFERRED_VERSION_vulkan-headers = "1.2.196"
PREFERRED_VERSION_vulkan-loader = "1.2.196"

WARN_QA_append = "ldflags installed-vs-shipped already-stripped pkgvarcheck file-rdeps"
ERROR_QA_remove = "ldflags installed-vs-shipped already-stripped pkgvarcheck file-rdeps"

HOSTTOOLS_NONFATAL += "bc cc c++ bison cmake python repo"

#glib-mkenums"

VIRTUAL-RUNTIME_dtcpmgr = ""

PREFERRED_VERSION_gmp_dunfell = "6.2.%"

AML_RDK_PREBUILT_SITE ?= "http://10.18.11.6/shanghai/image/linux-distro/Public/RDK/prebuilt/"
# For dm-verity
IMAGE_CLASSES += "${@bb.utils.contains('DISTRO_FEATURES', 'dm-verity', 'image_types aml-dm-verity-img', '', d)}"

# Sytem partition
DM_VERITY_IMAGE = "lib32-rdk-generic-mediaclient-image"
# vendor partition
VENDOR_DM_VERITY_IMAGE = "vendor-image"

DM_VERITY_IMAGE_TYPE = "ext4"
STAGING_VERITY_DIR = "${DEPLOY_DIR_IMAGE}"

# YouTube Cobalt Version
PREFERRED_VERSION_libcobalt = "22%"

# AVB is enabled by default on RDK platform, 
# if want to disable on specific machine, DISTRO_FEATURES_remove = "AVB" in the machine config
DISTRO_FEATURES_append = " AVB "
