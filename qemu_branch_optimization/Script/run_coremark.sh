#!/bin/bash

path_MB="../Benchmark/coremark_v1.0/"
path_qemu="../../build.qemu/i386-linux-user/qemu-i386"
path_qemuHW="../../build.qemu.hw/i386-linux-user/qemu-i386"

tclist=("./coremark.exe 0x0 0x0 0x66 0 7 1 2000"\
		"./coremark.exe 0x3415 0x3415 0x66 0 7 1 2000")

cd ${path_MB}
for tc in "${tclist[@]}"
do
	echo ------testcase:    ------
	echo ${tc}
	echo qemu_origin
	time ${path_qemu} ${tc} >run1.out
	echo qemu_opt
	time ${path_qemuHW} ${tc} >run2.out
	echo ------     end     ------
	cat *.out
done
rm *.out
