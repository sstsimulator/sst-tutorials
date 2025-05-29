#!/usr/bin/env bash
set -euo pipefail
set -x

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

CORE_BRANCH=devel
ELEMENTS_BRANCH=devel
HGCC_BRANCH=devel

# Core
cd $SCRIPT_DIR/sst-core
./autogen.sh
./configure --disable-mpi --prefix=$SCRIPT_DIR/install
make -j8 install
make clean

# Elements
cd $SCRIPT_DIR/sst-elements

#Skip some elements to keep container size manageable
for elem in ariel balar cacheTracer cassini cramSim gensa golem kingsley llyr miranda messier opal osseous prospero rdmaNic samba serrano shogun simpleSimulation thornhill vanadis vaultsim zodiac;
do
	cd src/sst/elements/$elem
	touch .ignore
	cd $SCRIPT_DIR/sst-elements
done

./autogen.sh
./configure --prefix=$SCRIPT_DIR/install
make -j8 install
make clean

# HGCC
cd $SCRIPT_DIR/sst-hgcc
./autogen.sh
mkdir build
cd build

CXX=/usr/bin/g++
CC=/usr/bin/gcc
../configure --with-std=17 \
    --prefix=$SCRIPT_DIR/install \
    --with-sst-elements=$SCRIPT_DIR/install \
    --with-sst-core=$SCRIPT_DIR/install

make -j8 install
