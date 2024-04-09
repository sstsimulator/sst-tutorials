#!/usr/bin/env bash
set -euo pipefail
set -x

CORE_BRANCH=master
ELEMENTS_BRANCH=devel

# See: https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html
PIN=pin-3.28-98749-g6643ecee5-gcc-linux

mkdir -p sst
cd sst

# Core
if [ ! -d "./sst-core" ];
then
    git clone https://github.com/sstsimulator/sst-core.git
    cd sst-core
    git switch $CORE_BRANCH
    cd ..
else
    echo "The sst-core directory already exists. Skipping download."
fi

# Elements
if [ ! -d "./sst-elements" ];
then
    git clone https://github.com/sstsimulator/sst-elements.git
    cd sst-elements
    git switch $ELEMENTS_BRANCH
    cd ..
else
    echo "The sst-elements directory already exists. Skipping download."
fi

if [ ! -d "./intel-pin" ];
then
    wget https://software.intel.com/sites/landingpage/pintool/downloads/$PIN.tar.gz
    tar xvzf $PIN.tar.gz
    mv $PIN intel-pin
    rm $PIN.tar.gz
    cd  ..
else
    echo "The pin-3.28 directory already exists. Skipping download."
fi
