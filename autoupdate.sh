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
cd ..
echo "boost up to date"

echo "Compiling rpclib..."
cd rpclib
#fixing the too high requirement for cmake in travis-ci by removing the minimum requirmeent line
tail -n +2 CMakeLists.txt > CMakeLists.txt.temp
cat CMakeLists.txt.temp > CMakeLists.txt
rm CMakeLists.txt.temp
#end fix
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
