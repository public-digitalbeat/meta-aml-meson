
#SRC_URI = "git://${AML_GIT_ROOT}${AML_GIT_ROOT_YOCTO_SUFFIX}/rdk/prebuilt/hosttools;protocol=${AML_GIT_PROTOCOL};branch=open-bsp-master"
SRC_URI = "git://${AML_GIT_ROOT}${AML_GIT_ROOT_YOCTO_SUFFIX}/rdk-prebuilt-hosttools;protocol=${AML_GIT_PROTOCOL};branch=open-bsp-master"
SRCREV ?= "${AUTOREV}"
PV = "${SRCPV}"
