#!/bin/bash
ARCH=arm CROSS_COMPILE=arm-linux- make distclean
ARCH=arm CROSS_COMPILE=arm-linux- make mrproper 
ARCH=arm CROSS_COMPILE=arm-linux- make val_defconfig 
ARCH=arm CROSS_COMPILE=arm-linux- make dtbs 
ARCH=arm CROSS_COMPILE=arm-linux- make -j8
ARCH=arm CROSS_COMPILE=arm-linux- make tar-pkg
./mkupdatepkg
#scp upgrade-*.tar.xz root@10.1.30.4:/root/
#ssh root@10.1.30.4 'sync'
exit 0
