#!/bin/bash

rm ../../bin/times.data

for def_param in `seq 1.0 .05 2.5`; do
	echo $def_param
	# custom compile files.c
	make clean
	gcc -c files.c -o files.o -Wall -I./ -I../common -O3 -DCUS_BETA=$def_param
	make
	# launch server in background
	# and client in foreground
	cd ../../bin/
	printf "%lf;" $def_param >> times.data
	./serveur1-Renotte_Claes 2000 &
	#give server time to load
	sleep 0.3
	/usr/bin/time -o times.data -a -f '%E' ./client1 192.168.1.10 2000 50M.bin 0
	cd ../src/sc1
done
