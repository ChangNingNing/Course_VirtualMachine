#!/bin/bash

path="../MyTestCase/"

cd ${path}
make

tclist=$(ls *32)
for tc in $tclist
do
	echo ------testcase: $tc------
	echo qemu_origin
	time ../build.qemu/i386-linux-user/qemu-i386 ./$tc > log1.out
	echo qemu_opt
	time ../build.qemu.hw/i386-linux-user/qemu-i386 ./$tc > log2.out
	echo --- diff
	diff log1.out log2.out
	echo ------     end     ------
done

rm *.out
make clean
