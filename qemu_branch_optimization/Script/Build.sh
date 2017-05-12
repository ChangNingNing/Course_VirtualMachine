#!/bin/bash

bf_ori="build.qemu"
bf_hw="build.qemu.hw"
bconfig="../qemu-0.13.0/configure --target-list=i386-linux-user"


cd ..
tar -xf qemu_virtual_machine.tar.bz2
mkdir ${bf_ori}
mkdir ${bf_hw}

cd ${bf_ori}
${bconfig}
make
cd ..

cd ${bf_hw}
cp ../src/* ../qemu-0.13.0/.
${bconfig}
make
cd ..

tar xvf Benchmark.tar
