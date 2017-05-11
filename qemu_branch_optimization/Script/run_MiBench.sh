#!/bin/bash

path_MB="../Benchmark/MiBench/automotive"
path_qemu="../../../build.qemu/i386-linux-user/qemu-i386"
path_qemuHW="../../../build.qemu.hw/i386-linux-user/qemu-i386"

tclist=("./basicmath/basicmath_small"\
		"./basicmath/basicmath_large"\
		"./bitcount/bitcnts 1125000"\
		"./qsort/qsort_small ./qsort/input_small.dat"\
		"./qsort/qsort_large ./qsort/input_large.dat"\
		"./susan/susan ./susan/input_large.pgm ./output.pgm -s"\
		"./susan/susan ./susan/input_large.pgm ./output.pgm -e"\
		"./susan/susan ./susan/input_large.pgm ./output.pgm -c")

cd ${path_MB}
for tc in "${tclist[@]}"
do
	echo ------testcase: ------
	echo ${tc}
	echo qemu_origin
	time ${path_qemu} ${tc} >output1.out
	echo qemu_opt
	time ${path_qemuHW} ${tc} >output2.out
	echo --- diff
	diff output1.out output2.out
	echo ------     end     ------
done

rm output*
