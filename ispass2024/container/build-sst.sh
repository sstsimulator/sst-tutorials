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
./autogen.sh
./configure --prefix=$SCRIPT_DIR/install --with-pin=$INTEL_PIN_DIRECTORY
make -j8 install
