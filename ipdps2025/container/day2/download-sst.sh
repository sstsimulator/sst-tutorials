#!/usr/bin/env bash
set -euo pipefail
set -x
unset SSH_ASKPASS

CORE_BRANCH=devel
ELEMENTS_BRANCH=devel
REV_BRANCH=devel
HGCC_BRANCH=devel

mkdir -p sst
cd sst

# Core
if [ ! -d "./sst-core" ];
then
    git clone --single-branch -b $CORE_BRANCH https://github.com/sstsimulator/sst-core.git
else
    echo "The sst-core directory already exists. Skipping download."
fi

# Elements
if [ ! -d "./sst-elements" ];
then
    git clone --single-branch -b $ELEMENTS_BRANCH https://github.com/sstsimulator/sst-elements.git
else
    echo "The sst-elements directory already exists. Skipping download."
fi

# Rev
if [ ! -d "./rev" ];
then
    git clone --single-branch -b $REV_BRANCH https://github.com/tactcomplabs/rev.git
else
    echo "The rev directory already exists. Skipping download."
fi

# GPGPUSim
if [ ! -d "./gpgpu-sim_distribution" ];
then
    git clone https://github.com/accel-sim/gpgpu-sim_distribution.git
else
    echo "The gpgpusim directory already exists. Skipping download."
fi

# Tutorial files. Remove before final release.
#if [ ! -d "./sst-tutorials" ];
#then
#    git clone https://github.com/sstsimulator/sst-tutorials.git
#else
#    echo "The sst-tutorial directory already exists. Skipping download."
#fi
