#!/bin/bash

max_file=20
min_file=1
cur_file=1

n_start=50000
n_end=1000000
n_step=50000

echo "Serial:"

./serial.out $N data/serial.txt $p

echo ""

echo "Parallel:"

for ((p=2; p <= 6; p = $[ $p+1 ]))
do
for ((t=200; t <= 1000; t = $[ $t+200 ]))
do
cur_file=$min_file
for ((n=$n_start; n <= $n_end; n = $[ $n+$n_step ]))
do
echo "Threads = $p Threshold = $t  N = $n"
./parallel.out $p $t data/$cur_file.txt
cur_file=$[ $cur_file+1 ]
done
done
done

echo ""

