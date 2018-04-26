#!/bin/bash
ARCH=arm CROSS_COMPILE=arm-linux- make distclean
ARCH=arm CROSS_COMPILE=arm-linux- make mrproper 
ARCH=arm CROSS_COMPILE=arm-linux- make val_defconfig 
ARCH=arm CROSS_COMPILE=arm-linux- make dtbs 
ARCH=arm CROSS_COMPILE=arm-linux- make -j8
ARCH=arm CROSS_COMPILE=arm-linux- make tar-pkg
#rm -rf tar-install/lib/modules/4.1.15/source
#rm -rf tar-install/lib/modules/4.1.15/build
#ssh root@10.1.30.4 'rm -rf /lib/modules/*'
#scp -r tar-install/lib/modules/4.1.15 root@10.1.30.4:/lib/modules/
#scp -r arch/arm/boot/zImage  root@10.1.30.4:/boot/
#scp -r arch/arm/boot/dts/imx6q-val100.dtb  root@10.1.30.4:/boot/
#ssh root@10.1.30.4 'sync;reboot'
#cp arch/arm/boot/zImage ~/work/imx_usb_loader/firmware/
#cp arch/arm/boot/dts/ATR200B.dtb ~/work/imx_usb_loader/firmware/
exit 0
