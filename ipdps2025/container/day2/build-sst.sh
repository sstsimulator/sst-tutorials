#!/usr/bin/env bash
set -euo pipefail
set -x

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

CORE_BRANCH=devel
ELEMENTS_BRANCH=devel
REV_BRANCH=devel

# GPGPU-Sim
cd $SCRIPT_DIR/gpgpu-sim_distribution
cmake -B build
cmake --build build -j4
cmake --install build

# Core
cd $SCRIPT_DIR/sst-core
./autogen.sh
./configure --disable-mpi --prefix=$SCRIPT_DIR/install
make -j8 install
make clean

# Elements
cd $SCRIPT_DIR/sst-elements

#Skip some elements to keep container size manageable
for elem in ariel cacheTracer cramSim ember firefly gensa golem llyr messier opal osseous prospero rdmaNic samba serrano thornhill vaultsim zodiac;
do
	cd src/sst/elements/$elem
	touch .ignore
	cd $SCRIPT_DIR/sst-elements
done

./autogen.sh
./configure --prefix=$SCRIPT_DIR/install --with-sst-core=$SCRIPT_DIR/install --with-cuda=/usr/local/cuda --with-gpgpusim=/home/jammy/sst/gpgpu-sim_distribution
make -j8 install
make clean

# Rev
cd $SCRIPT_DIR/rev
git config core.hooksPath .githooks
cd $SCRIPT_DIR/rev/build
cmake -DRVCC=/opt/riscv/bin/riscv64-unknown-elf-gcc -DRVCXX=/opt/riscv/bin/riscv64-unknown-elf-g++ ..
make -j8
make install
