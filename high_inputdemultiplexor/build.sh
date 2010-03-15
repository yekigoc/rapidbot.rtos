#!/bin/bash
make distclean
gtkdocize || exit 1
cd lib
bash ./pyprotoc.sh
cd ../
autoreconf --install;
./configure --prefix=/home/snegovick/build/usr --enable-gtk-doc
 make clean; make && make install
