#!/bin/sh

PATH=/sbin:/bin:/usr/sbin:/usr/bin

ROOT_MOUNT="/rootfs"
INIT="/sbin/init"
ROOT_DEVICE="/dev/system"
VENDOR_DEVICE="/dev/vendor"
ROOT_RWDEVICE="/dev/data"
MOUNT="/bin/mount"
UMOUNT="/bin/umount"
FIRMWARE=""
DM_VERITY_STATUS="disabled"
DM_DEV_COUNT=0
ACTIVE_SLOT=""
RW_MOUNTDIR="/data"
OVERLAY_DIR="/data/overlay"
VBMETA_DEVICE=""

# Copied from initramfs-framework. The core of this script probably should be
# turned into initramfs-framework modules to reduce duplication.
udev_daemon() {
	OPTIONS="/sbin/udev/udevd /sbin/udevd /lib/udev/udevd /lib/systemd/systemd-udevd"

	for o in $OPTIONS; do
		if [ -x "$o" ]; then
			echo $o
			return 0
		fi
	done

	return 1
}

_UDEV_DAEMON=`udev_daemon`

early_setup() {
    mkdir -p /proc
    mkdir -p /sys
    mount -t proc proc /proc
    mount -t sysfs sysfs /sys
    mount -t devtmpfs none /dev

    mkdir -p /run
    mkdir -p /var/run

    $_UDEV_DAEMON --daemon
    udevadm trigger --action=add
}

read_args() {
    [ -z "$CMDLINE" ] && CMDLINE=`cat /proc/cmdline`
    for arg in $CMDLINE; do
        optarg=`expr "x$arg" : 'x[^=]*=\(.*\)'`
        case $arg in
            root=*)
                ROOT_DEVICE=$optarg ;;
            rootfstype=*)
                modprobe $optarg 2> /dev/null ;;
            androidboot.slot_suffix=*)
                ACTIVE_SLOT=$optarg
                ROOT_DEVICE=${ROOT_DEVICE}${ACTIVE_SLOT};;
            androidboot.vbmeta.device=*)
                VBMETA_DEVICE=$optarg ;;
            LABEL=*)
                label=$optarg ;;
            video=*)
                video_mode=$arg ;;
            vga=*)
                vga_mode=$arg ;;
            console=*)
                if [ -z "${console_params}" ]; then
                    console_params=$arg
                else
                    console_params="$console_params $arg"
                fi ;;
            firmware=*)
                FIRMWARE=$optarg ;;
            init=*)
                init=$optarg ;;
            debugshell*)
                if [ -z "$optarg" ]; then
                        shelltimeout=30
                else
                        shelltimeout=$optarg
                fi
        esac
    done
}

boot_root() {
    # Watches the udev event queue, and exits if all current events are handled
    udevadm settle

    # The rootfs does not yet contain kernel modules.  Copy it!
    if [ ! -d ${ROOT_MOUNT}/lib/modules ];
    then
        cp -rf /lib/modules ${ROOT_MOUNT}/lib/
        cp -rf /lib/firmware ${ROOT_MOUNT}/lib/
        cp -rf /etc/modprobe.d ${ROOT_MOUNT}/etc/
        cp -rf /etc/modules-load.d ${ROOT_MOUNT}/etc/
        cp -rf /etc/modules ${ROOT_MOUNT}/etc/
	fi

    mount -n --move /proc ${ROOT_MOUNT}/proc
    mount -n --move /sys ${ROOT_MOUNT}/sys
    mount -n --move /dev ${ROOT_MOUNT}/dev

    if [ "$DM_VERITY_STATUS" = "disabled" ] && [ "${ACTIVE_SLOT}" != "" ]; then
        slot=$(cat ${ROOT_MOUNT}/etc/fstab | grep -E "/dev/vendor" | awk '{print $1}' | cut -c 12-)
        if [ "${ACTIVE_SLOT}" != "${slot}" ] && [ "$DM_VERITY_STATUS" != "enabled" ]; then
            echo "switch vendor${slot} to vendor${ACTIVE_SLOT}"
            sed -i "s/vendor\\${slot}/vendor\\${ACTIVE_SLOT}/" ${ROOT_MOUNT}/etc/fstab
        fi
    fi

    cd $ROOT_MOUNT

    # busybox switch_root supports -c option
    exec switch_root -c /dev/console $ROOT_MOUNT $INIT ||
        fatal "Couldn't switch_root, dropping to shell"
}

fatal() {
    echo $1 >$CONSOLE
    echo >$CONSOLE
    exec sh
}

early_setup

[ -z "$CONSOLE" ] && CONSOLE="/dev/console"

read_args

#Waiting for device to become ready

wait_for_device () {
    i=1
    while [ "$i" -le 30 ]
    do
        if [ -b "${ROOT_DEVICE}" ]; then
            echo "${ROOT_DEVICE} is ready now."
            break
        fi
        echo "${ROOT_DEVICE} is not ready.  Waited for ${i} second"
        sleep 1
        i=$((i+1))
    done
}

format_and_install() {
    if [ -f "/${ROOT_MOUNT}/${FIRMWARE}" ] ; then
        echo "formating file system"
        export LD_LIBRARY_PATH=/usr/lib
        umount /dev/system
        mkfs.ext4 -F /dev/system
        mkdir -p system
        if ! mount -o rw,noatime,nodiratime -t ext4 /dev/system /system ; then
            fatal "Could not mount system device"
        fi
        echo "extracting file system ..."
        gunzip -c /${ROOT_MOUNT}/${FIRMWARE} | tar -xf - -C /system
        if [ $? -ne 0 ]; then
            echo "Error: untar failed."
        else
            echo "Done"
        fi
        device=/dev/boot
        if [ -f "/${ROOT_MOUNT}/boot.img" ]; then
            echo "Writing boot.img into boot partition(${device})..."
            dd if=/${ROOT_MOUNT}/boot.img of=${device}
            echo "Done"
        fi
        sync
        echo "copying existing modules to rootfs"
        cp -rf /lib/modules /system/lib/
        cp -rf /lib/firmware /system/lib/
        cp -rf /etc/modprobe.d /system/etc/
        cp -rf /etc/modules-load.d /system/etc/
        cp -rf /etc/modules /system/etc/
        echo "update complete"
        umount $ROOT_MOUNT
        ROOT_DEVICE=/dev/system
        ROOT_MOUNT=/system
    else
        echo "cannot locate ${FIRMWARE}"
        echo "boot normally..."
    fi
}
dm_verity_setup() {
    echo "setup dm-verity for ${1} partition(${2}) mount to ${3}"
    VERITY_ENV=/usr/share/${1}-dm-verity.env

    VBMETA_DEVICE_REAL=${VBMETA_DEVICE}${ACTIVE_SLOT}
    # Change /dev/block/ to /dev/
    if [ ! -b ${VBMETA_DEVICE_REAL} ]; then
        VBMETA_DEVICE_REAL=`echo ${VBMETA_DEVICE_REAL} | sed "s/\/block\//\//g"`
    fi
    if [ -b "${VBMETA_DEVICE_REAL}" ]; then
        mkdir -p /tmp
        AVB_DM_TOOL=/usr/bin/avbtool-dm-verity.py
        if [ -x ${AVB_DM_TOOL} ]; then
            VERITY_ENV=/tmp/${1}-dm-verity.env
            ${AVB_DM_TOOL} print_partition_verity --image "${VBMETA_DEVICE_REAL}" --partition_name "${1}" --active_slot "${ACTIVE_SLOT}" --output "$VERITY_ENV"
            if [ "$?" != "0" ]; then
                echo "failed to read vbmeta device from ${VBMETA_DEVICE_REAL}"
            fi
        fi
    fi

    echo "verity env is $VERITY_ENV"

    if [ -f $VERITY_ENV ]; then
        . $VERITY_ENV
        veritysetup --data-block-size=${DATA_BLOCK_SIZE} --hash-offset=${DATA_SIZE} \
            create ${1} ${2} ${2} ${ROOT_HASH}
        if [ $? = 0 ]; then
            if [ "${3}" != "none" ]; then
                #mount -o ro /dev/mapper/${1} ${3}
                mount -o ro /dev/dm-${DM_DEV_COUNT} ${3}
            else
                echo "skip mounting ${2}"
            fi
            DM_VERITY_STATUS="enabled"
            DM_DEV_COUNT=$((DM_DEV_COUNT+1))
        else
            echo "dm-verity fails with return code $?"
            DM_VERITY_STATUS="disabled"
        fi
    else
        echo "Cannot find root hash in initramfs"
        DM_VERITY_STATUS="disabled"
    fi
}

data_ext4_handle() {
    echo -e "Partition formater on $ROOT_RWDEVICE"
    FsType=$(blkid $ROOT_RWDEVICE | sed -n 's/.*TYPE=\"\([^\"]*\)\".*/\1/p')
    if [ "${FsType}" != "ext4" ]; then
        echo -e "Formating $ROOT_RWDEVICE to ext4 ..."
        yes 2>/dev/null | mkfs.ext4 -q -m 0 $ROOT_RWDEVICE
        sync
        FsType=$(blkid $ROOT_RWDEVICE | sed -n 's/.*TYPE=\"\([^\"]*\)\".*/\1/p')
        echo -e "After formating FSTYPE of $ROOT_RWDEVICE = ${FsType} ..."
    fi

    [ ! -d $1 ] && mkdir -p $1
    if ! mount -t ext4 -o rw,noatime,nodiratime $ROOT_RWDEVICE $1 ; then
        fatal "Could not mount $ROOT_RWDEVICE"
    fi
}

# Try to mount the root image read-write and then boot it up.
# This function distinguishes between a read-only image and a read-write image.
# In the former case (typically an iso), it tries to make a union mount if possible.
# In the latter case, the root image could be mounted and then directly booted up.
mount_and_boot() {
    mkdir $ROOT_MOUNT

    mknod /dev/loop0 b 7 0 2>/dev/null

    if [ "${FIRMWARE}" != "" ];
    then
	ROOT_DEVICE="/dev/mmcblk1p1"
	wait_for_device
    fi
    if [ "$ROOT_DEVICE" != "" ]; then
        dm_verity_setup system ${ROOT_DEVICE} ${ROOT_MOUNT}
        dm_verity_setup vendor ${VENDOR_DEVICE}${ACTIVE_SLOT} none

        echo "dm-verity is $DM_VERITY_STATUS"

        if [ "$DM_VERITY_STATUS" = "disabled" ]; then
            if ! mount -o rw,noatime,nodiratime $ROOT_DEVICE $ROOT_MOUNT ; then
                fatal "Could not mount rootfs device"
            fi
        fi
    fi

    if [ "${FIRMWARE}" != "" ]; then
	format_and_install
    fi

    if touch $ROOT_MOUNT/bin 2>/dev/null || [ -e $ROOT_MOUNT/read-only ]; then
        data_ext4_handle $ROOT_MOUNT/data
        # The root image is read-write, directly boot it up.
        boot_root
    fi

    # determine which unification filesystem to use
    union_fs_type=""
    if grep -q -w "overlay" /proc/filesystems; then
        union_fs_type="overlay"
    elif grep -q -w "aufs" /proc/filesystems; then
        union_fs_type="aufs"
    else
        union_fs_type=""
    fi

    # make a union mount if possible
    case $union_fs_type in
	"overlay")
	    mkdir -p /rootfs.ro $RW_MOUNTDIR
	    if ! mount -n --move $ROOT_MOUNT /rootfs.ro; then
            rm -rf /rootfs.ro $RW_MOUNTDIR
            fatal "Could not move rootfs mount point"
        else 
            # mount /dev/data as overlay fs
            if [ -b "$ROOT_RWDEVICE" ]; then
                data_ext4_handle $RW_MOUNTDIR
            else
				echo "data partition not exist"
                mount -t tmpfs -o rw,noatime,mode=755 tmpfs $RW_MOUNTDIR
            fi
            mkdir -p $OVERLAY_DIR/upperdir $OVERLAY_DIR/work
            mount -t overlay overlay -o "lowerdir=/rootfs.ro,upperdir=$OVERLAY_DIR/upperdir,workdir=$OVERLAY_DIR/work" $ROOT_MOUNT
            mkdir -p $ROOT_MOUNT/rootfs.ro $ROOT_MOUNT/$RW_MOUNTDIR
            mount --move /rootfs.ro $ROOT_MOUNT/rootfs.ro
            mount --move $RW_MOUNTDIR $ROOT_MOUNT/$RW_MOUNTDIR
        fi
	    ;;
	"aufs")
	    mkdir -p /rootfs.ro /rootfs.rw
	    if ! mount -n --move $ROOT_MOUNT /rootfs.ro; then
		rm -rf /rootfs.ro /rootfs.rw
		fatal "Could not move rootfs mount point"
	    else
		mount -t tmpfs -o rw,noatime,mode=755 tmpfs /rootfs.rw
		mount -t aufs -o "dirs=/rootfs.rw=rw:/rootfs.ro=ro" aufs $ROOT_MOUNT
		mkdir -p $ROOT_MOUNT/rootfs.ro $ROOT_MOUNT/rootfs.rw
		mount --move /rootfs.ro $ROOT_MOUNT/rootfs.ro
		mount --move /rootfs.rw $ROOT_MOUNT/rootfs.rw
	    fi
	    ;;
	"")
	    mount -t tmpfs -o rw,noatime,mode=755 tmpfs $ROOT_MOUNT/media
	    ;;
    esac

    # boot the image
    boot_root
}

wait_for_device
mount_and_boot
