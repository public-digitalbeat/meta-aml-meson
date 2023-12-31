#@TYPE: Machine
#@NAME: meson

#@DESCRIPTION: Machine configuration for meson systems

include conf/machine/include/mesonsc2_5.4.inc

PREFERRED_PROVIDER_virtual/egl = "libgles-eabihf-dvalin-wayland-drm"
PREFERRED_PROVIDER_virtual/libgl = "libgles-eabihf-dvalin-wayland-drm"
PREFERRED_PROVIDER_virtual/libgles1 = "libgles-eabihf-dvalin-wayland-drm"
PREFERRED_PROVIDER_virtual/libgles2 = "libgles-eabihf-dvalin-wayland-drm"

MACHINE_ESSENTIAL_EXTRA_RRECOMMENDS += " \
        wififw-qca6174 \
        qca6174 \
        aucpu-fw \
        "
#ap6xxx-pcie
#ap6xxx-sdio
#brcm-tools
#wififw-ap6398
#tuner-prebuilt 

KERNEL_DEVICETREE = "sc2_s905x4_ah212_linux.dtb"
KERNEL_MODULE_AUTOLOAD += " btqca"
KERNEL_MODULE_PROBECONF += "btqca"
KERNEL_MODULE_AUTOLOAD += " hci_uart"
KERNEL_MODULE_PROBECONF += "hci_uart"
KERNEL_MODULE_AUTOLOAD += "cxd2856_fe_64"
KERNEL_MODULE_AUTOLOAD += "aml_aucpu"
KERNEL_MODULE_AUTOLOAD += "meson_ir"
KERNEL_MODULE_AUTOLOAD += "snd-soc-dummy_codec"
KERNEL_MODULE_AUTOLOAD += "snd-soc-aml_t9015"
KERNEL_MODULE_AUTOLOAD += "dolby_fw"
KERNEL_MODULE_AUTOLOAD += "meson-cpufreq"
KERNEL_MODULE_AUTOLOAD += " media_sync"

UBOOT_MACHINE = "sc2_ah212_config"
DISTRO_FEATURES_append = " drm"
DISTRO_FEATURES_append = " onepass"
DISTRO_FEATURES_append = " secure-u-boot optee "
DISTRO_FEATURES_append = " absystem"
DISTRO_FEATURES_append = " opencdm "
#DISTRO_FEATURES_append = " widevine "
#DISTRO_FEATURES_append = " playready "
#DISTRO_FEATURES_append = " amlogic-dvb "
DISTRO_FEATURES_append = " bluetooth bluez5"
DISTRO_FEATURES_append = " ptest "
DISTRO_FEATURES_remove = " alsa "
OVERRIDES .= ":ah212:sc2:onepass"
MACHINEOVERRIDES .= ":client"
OVERRIDES .= ":dtv"
#DISTRO_FEATURES_append = " dvb"
#DISTRO_FEATURES_append = " aml-iptv"
PREFERRED_PROVIDER_widevine = "aml-mediadrm-widevine"

PREFERRED_VERSION_wayland = "1.17.0"
PREFERRED_VERSION_wayland-protocol = "1.18.0"

DISTRO_FEATURES_remove_dunfell = " virtualization"

# AVB and chained partition
#DISTRO_FEATURES_append = " AVB_chained_partition "
#DEVICE_PROPERTY_SYSTEM_ROLLBACK_LOCATION = "1"
#DEVICE_PROPERTY_VENDOR_ROLLBACK_LOCATION = "2"
# For AVB2 chained partition, the actual signing and root hash information
# are stored in the AVB2 footer of the partition
# Libavb in U-Boot will look at the end of the partition to look for footer.
# Therefore, exact size of the partition has to be specified.
#DEVICE_PROPERTY_SYSTEM_PARTITION_SIZE = "0x40000000"
#DEVICE_PROPERTY_VENDOR_PARTITION_SIZE = "0x1000000"

#Xconf Firmware Upgrade
XCONF_MODELNAME = "AH212"
