#!/bin/bash

echo "Getting dependencies..."

git submodule init
git submodule update

cd orencash/src/dependencies

wget https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz
echo "Extracting boost..."
tar xvzf boost_1_65_1.tar.gz
echo "Cleaning up"
rm boost_1_65_1.tar.gz 
cd boost_1_65_1
sh bootstrap.sh --prevfix=/usr/
./b2  --with-serialization
./b2 install
cd ..
echo "boost up to date"

echo "Compiling rpclib..."
cd rpclib
mkdir build
cd build
cmake ..
make
make install
cd ..
cd ..
echo "rpclib up to date"

echo "Compiling cryptopp..."
cd cryptopp
make
make install
cd ..
echo "cryptopp up to date"

echo "Compiling restbed..."
cd restbed
git submodule init
git submodule update
mkdir build
cd build
cmake -DBUILD_SSL=OFF ..
make
make install
cd ..
cd ..
echo "restbed up to date"
