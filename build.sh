#!/bin/bash
ARCH=arm CROSS_COMPILE=arm-linux- make distclean
ARCH=arm CROSS_COMPILE=arm-linux- make mrproper 
ARCH=arm CROSS_COMPILE=arm-linux- make swallow_defconfig 
ARCH=arm CROSS_COMPILE=arm-linux- make dtbs 
ARCH=arm CROSS_COMPILE=arm-linux- make -j8
cp arch/arm/boot/zImage ~/work/imx_usb_loader/firmware/
cp arch/arm/boot/dts/imx6dl-s16.dtb ~/work/imx_usb_loader/firmware/
exit 0
