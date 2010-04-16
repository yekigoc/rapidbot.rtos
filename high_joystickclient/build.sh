#!/bin/bash
make distclean
autoreconf --install;
./configure --prefix=/home/snegovick/build/usr
 make clean; make && make install
