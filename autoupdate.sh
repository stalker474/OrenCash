#!/bin/bash
set -e #exit on error
echo "Getting dependencies..."

git submodule init
git submodule update

cd orencash/src/dependencies
echo "Compiling boost..."
cd boost
sh bootstrap.sh --prefix=/usr/
./b2  --with-serialization > /dev/null
sudo ./b2 install > /dev/null
cd ..
echo "boost up to date"

echo "Compiling rpclib..."
cd rpclib
mkdir build
cd build
sudo cmake ..
make
sudo make install
cd ..
cd ..
echo "rpclib up to date"

echo "Compiling cryptopp..."
cd cryptopp
make
sudo make install
cd ..
echo "cryptopp up to date"

echo "Compiling restbed..."
cd restbed
git submodule init
git submodule update
mkdir build
cd build
sudo cmake -DBUILD_SSL=OFF ..
make
sudo make install
sudo mv librestbed.a /usr/lib
cd ..
cd ..
echo "restbed up to date"
