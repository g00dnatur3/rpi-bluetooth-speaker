#!/bin/bash --debugger
set -e

sudo apt-get install --yes --no-install-recommends libsndfile1 libsndfile1-dev

cd $HOME
[ ! -d sbc ] && mkdir sbc
cd sbc

[ ! -d sbc-1.3 ] && wget https://www.kernel.org/pub/linux/bluetooth/sbc-1.3.tar.xz && tar xvf sbc-1.3.tar.xz && rm -rf sbc-1.3.tar.xz

cd sbc-1.3

./configure

make

sudo make install 

