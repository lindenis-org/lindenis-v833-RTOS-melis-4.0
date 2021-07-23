#!/bin/sh
mv rootfs/res/fonts/ temp_fonts/
mv rootfs/res/sounds/ temp_sounds/
rm -rf rootfs/res/*
mv temp_fonts/ rootfs/res/fonts/
mv temp_sounds/ rootfs/res/sounds/

rm ./rootfs/drv/sample.drv

