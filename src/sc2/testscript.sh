#!/bin/bash

rm ../../bin/times.data

for def_param in `seq 0.8 0.02 0.95`; do
	echo $def_param
	# custom compile files.c
	make clean
	gcc -c files.c -o files.o -Wall -I./ -I../common -O3 -DCWND_FAC=$def_param
	make
	# launch server in background
	# and client in foreground
	cd ../../bin/
	printf "%lf;" $def_param >> times.data
	./serveur2-Renotte_Claes 2000 &
	#give server time to load
	sleep 0.3
	/usr/bin/time -o times.data -a -f '%E' ./client2 192.168.1.10 2000 20M.bin 0
	cd ../src/sc2
done
