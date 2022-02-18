# !/bin/bash

if [ ! -d "build" ]; then
	mkdir build && cd build
else 
	cd build
fi

cmake .. > /dev/null

if [ ! -x "Netlib" ]; then
	make -j 4 > /dev/null
	clear
	echo "make Successfully"
else 
	make clean
	make -j 4 > /dev/null
	clear
	echo "make Successfully"
fi

./Netlib