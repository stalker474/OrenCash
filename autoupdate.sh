#!/bin/bash
set -e #exit on error
echo "Getting dependencies..."

git submodule init
git submodule update

cd orencash/src/dependencies

echo "Compiling rpclib..."
cd rpclib
mkdir -p build
cd build
cmake ..
make
cd ..
cd ..
echo "rpclib up to date"

echo "Compiling cryptopp..."
cd cryptopp
make
cd ..
echo "cryptopp up to date"

echo "Compiling restbed..."
cd restbed
git submodule init
git submodule update
mkdir -p build
cd build
cmake -DBUILD_SSL=OFF ..
make
cd ..
cd ..
echo "restbed up to date"

echo "preparing libraries"
cd ..
mkdir -p "libs"
mv dependencies/cryptopp/*.a libs/
mv dependencies/restbed/build/*.a libs/
mv dependencies/rpclib/build/*.a libs/
