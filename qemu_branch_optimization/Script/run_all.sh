#!/bin/bash

script=("./run_MyTestCase.sh"\
		"./run_MiBench.sh"\
		"./run_coremark.sh")

rm log.txt
for i in ${script[2]}
do
	echo ${i}
	for((j=0; j<10; j+=1))
	do
		echo ${j}
		${i} 1>>log.txt 2>>log.txt
	done
done
