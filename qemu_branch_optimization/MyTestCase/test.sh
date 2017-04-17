#!/bin/bash

tclist=$(ls *32)
for tc in $tclist
do
	echo ------testcase: $tc------
	echo qemu_origin
	time ../build.qemu/i386-linux-user/qemu-i386 ./$tc
	echo qemu_opt
	time ../build.qemu.hw/i386-linux-user/qemu-i386 ./$tc
	echo ------     end     ------
done

