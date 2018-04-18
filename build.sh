#!/bin/bash
ARCH=arm CROSS_COMPILE=arm-linux- make distclean
ARCH=arm CROSS_COMPILE=arm-linux- make mrproper 
ARCH=arm CROSS_COMPILE=arm-linux- make val_defconfig 
ARCH=arm CROSS_COMPILE=arm-linux- make dtbs 
ARCH=arm CROSS_COMPILE=arm-linux- make -j8
#cp arch/arm/boot/zImage ~/work/imx_usb_loader/firmware/
#cp arch/arm/boot/dts/ATR200B.dtb ~/work/imx_usb_loader/firmware/
exit 0
