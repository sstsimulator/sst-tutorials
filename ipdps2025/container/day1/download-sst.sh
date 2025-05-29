#!/usr/bin/env bash
set -euo pipefail
set -x
unset SSH_ASKPASS

CORE_BRANCH=devel
ELEMENTS_BRANCH=devel

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

# Hgcc
if [ ! -d "./sst-hgcc" ];
then
    git clone --single-branch https://github.com/sstsimulator/sst-hgcc.git
else
    echo "The sst-hgcc directory already exists. Skipping download."
fi



