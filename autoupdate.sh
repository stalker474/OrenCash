#!/bin/bash
set -e #exit on error
echo "Getting dependencies..."

git submodule init
git submodule update

cd orencash/src/dependencies
echo "Compiling boost..."
cd boost
sh bootstrap.sh --prefix=/usr/
./b2  --with-serialization
sudo ./b2 install
cd ..
echo "boost up to date"

echo "Compiling rpclib..."
cd rpclib
mkdir build
cd build
sudo sed -i cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
make
sudo sed -i make install
cd ..
cd ..
echo "rpclib up to date"

echo "Compiling cryptopp..."
cd cryptopp
make
sudo sed -i make install
cd ..
echo "cryptopp up to date"

echo "Compiling restbed..."
cd restbed
git submodule init
git submodule update
mkdir build
cd build
sudo sed -i cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/lib -DBUILD_SSL=OFF ..
make
sudo sed -i make install
sudo sed -i mv librestbed.a /usr/lib
cd ..
cd ..
echo "restbed up to date"
