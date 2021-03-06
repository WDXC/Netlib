# !/bin/bash

if [ ! -d "build" ]; then
	mkdir build && cd build
else 
	cd build
fi

cmake .. > /dev/null

if [ ! -d "bin" ]; then
	make -j 8 > /dev/null
	echo "make Successfully"
else 
	make clean
	make -j 8 > /dev/null
	echo "make Successfully"
fi

cp ./src/libNet.a ../lib
