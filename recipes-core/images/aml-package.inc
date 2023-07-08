DEPENDS_append = " aml-img-packer-native"
DEPENDS_append_onepass = " core-image-minimal"
DEPENDS_append = " vendor-image"
DEPENDS_append = " coreutils-native"

SOC_FAMILY = "gxl"
SOC_FAMILY_gxl = "gxl"
SOC_FAMILY_axg = "axg"
SOC_FAMILY_txlx = "txlx"
SOC_FAMILY_g12a = "g12a"
SOC_FAMILY_g12b = "g12b"
SOC_FAMILY_tm2 = "tm2"
SOC_FAMILY_sc2 = "sc2"
SOC_FAMILY_t5d = "t5d"
SOC_FAMILY_s4 = "s4"
SOC_FAMILY_t7 = "t7"
SOC_FAMILY_t5w = "t5w"
SOC_FAMILY_t3 = "t3"

ABSYS = "${@bb.utils.contains('DISTRO_FEATURES','absystem','_ab','', d)}"
SBL33 = "${@bb.utils.contains('DISTRO_FEATURES','separate-bl33','_sbl33','', d)}"
PACK_CONF = "aml_upgrade_package${ABSYS}${SBL33}.conf"

padding_to_multipleOf4K_for_img2simg() {
    local add_size=0
    local file_size=$(stat -L -c "%s" $1)

    echo "$1 file_size $file_size"

    local remain=$(expr $file_size % 4096)
    if [ $remain -ne 0 ]; then
        add_size=$(expr 4096 - $remain)
        echo "$1 add_size $add_size"
        if [ $add_size -gt 0 ];then
            echo "$1 padding $add_size bytes for img2simg"
            truncate --size="+$add_size" $1
        fi
    fi
}

do_aml_pack_32b() {
       if ${@bb.utils.contains('DISTRO_FEATURES','onepass','false','true',d)}; then
            dir64b=`echo ${DEPLOY_DIR_IMAGE} | sed -r 's/_32b//'`
            if [ -e $dir64b ];then
                cp $dir64b/boot.img ${DEPLOY_DIR_IMAGE}
                cp $dir64b/dtb.img ${DEPLOY_DIR_IMAGE}/
                cp $dir64b/u-boot* ${DEPLOY_DIR_IMAGE}
            fi
        fi

    cp -f ${STAGING_DIR_NATIVE}${bindir}/aml-img-packer/aml_* ${DEPLOY_DIR_IMAGE}/
    cp -f ${STAGING_DIR_NATIVE}${bindir}/aml-img-packer/img2simg ${DEPLOY_DIR_IMAGE}/
    cp -f ${STAGING_DIR_NATIVE}${bindir}/aml-img-packer/res_packer ${DEPLOY_DIR_IMAGE}/
    cp -rf ${STAGING_DIR_NATIVE}${bindir}/aml-img-packer/${SOC_FAMILY}/* ${DEPLOY_DIR_IMAGE}/
    if ${@bb.utils.contains('DISTRO_FEATURES', 'AVB', 'true', 'false', d)}; then
        if [ "${ABSYS}" = "_ab" ];then
            sed -i '$a file="vbmeta.img"           main_type="PARTITION"      sub_type="vbmeta_a"' ${DEPLOY_DIR_IMAGE}/${PACK_CONF}
        else
            sed -i '$a file="vbmeta.img"           main_type="PARTITION"      sub_type="vbmeta"' ${DEPLOY_DIR_IMAGE}/${PACK_CONF}
        fi
    fi
    if ${@bb.utils.contains('DISTRO_FEATURES', 'recovery', 'false', 'true', d)}; then
        sed -i '/recovery/ d' ${DEPLOY_DIR_IMAGE}/${PACK_CONF}
    fi
    ${DEPLOY_DIR_IMAGE}/res_packer -r ${DEPLOY_DIR_IMAGE}/logo_img_files ${DEPLOY_DIR_IMAGE}/logo.img
    padding_to_multipleOf4K_for_img2simg "${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.ext4"
    ${DEPLOY_DIR_IMAGE}/img2simg ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.ext4 ${DEPLOY_DIR_IMAGE}/rootfs.ext4.img2simg
    if [ -f ${DEPLOY_DIR_IMAGE}/vendor-image-${MACHINE}.ext4 ];then
        padding_to_multipleOf4K_for_img2simg "${DEPLOY_DIR_IMAGE}/vendor-image-${MACHINE}.ext4"
        ${DEPLOY_DIR_IMAGE}/img2simg ${DEPLOY_DIR_IMAGE}/vendor-image-${MACHINE}.ext4 ${DEPLOY_DIR_IMAGE}/vendor.ext4.img2simg
    fi

    #if system have generate secureboot uboot, just use it.
    test -f ${DEPLOY_DIR_IMAGE}/u-boot.bin.device.signed && sed 's/"u-boot.bin.signed"/"u-boot.bin.device.signed"/' -i ${DEPLOY_DIR_IMAGE}/${PACK_CONF}

    ${DEPLOY_DIR_IMAGE}/aml_image_v2_packer_new -r ${DEPLOY_DIR_IMAGE}/${PACK_CONF} ${DEPLOY_DIR_IMAGE} ${DEPLOY_DIR_IMAGE}/aml_upgrade_package.img

}
do_aml_pack_32b[depends] += "core-image-minimal:do_image_complete "
do_aml_pack_32b[depends] += "vendor-image:do_image_complete "
do_aml_pack_32b[depends] += "${@bb.utils.contains('DISTRO_FEATURES', 'AVB', ' vbmeta-image:do_compile', '', d)}"
addtask aml_pack_32b after do_image_complete before do_build
do_aml_pack_32b[nostamp] = "1"
