#!/usr/bin/env bash
set -euo pipefail
set -x

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

CORE_BRANCH=master
ELEMENTS_BRANCH=devel

# Core
cd $SCRIPT_DIR/sst-core
./autogen.sh
./configure --disable-mpi --prefix=$SCRIPT_DIR/install
make -j8 install

# Elements
cd $SCRIPT_DIR/sst-elements

#for elem in balar ember firefly gensa hermes iris llyr mask-mpi mercury osseous samba simpleElementExample simpleSimulation thornhill vanadis zodiac;
for elem in ariel balar cacheTracer cramSim gensa iris llyr mask-mpi mercury messier mmu opal osseous rdmaNic samba serrano thornhill vaultsim zodiac;
do
	cd src/sst/elements/$elem
	touch .ignore
	cd $SCRIPT_DIR/sst-elements
done

./autogen.sh
#./configure --prefix=$SCRIPT_DIR/install --with-pin=$INTEL_PIN_DIRECTORY
./configure --prefix=$SCRIPT_DIR/install
make -j8 install
