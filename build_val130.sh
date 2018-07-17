#!/bin/bash
ARCH=arm CROSS_COMPILE=arm-linux- make distclean
ARCH=arm CROSS_COMPILE=arm-linux- make mrproper 
ARCH=arm CROSS_COMPILE=arm-linux- make val130_defconfig 
ARCH=arm CROSS_COMPILE=arm-linux- make dtbs 
ARCH=arm CROSS_COMPILE=arm-linux- make -j8
ARCH=arm CROSS_COMPILE=arm-linux- make tar-pkg
exit 0
