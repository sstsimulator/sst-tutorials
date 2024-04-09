#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export SST_INSTALL=${SCRIPT_DIR}/install
export INTEL_PIN_DIRECTORY=${SCRIPT_DIR}/pin-3.28
export INTEL_PIN_DIR=${INTEL_PIN_DIRECTORY}
export PIN_HOME=${INTEL_PIN_DIRECTORY}
export PATH=${SST_INSTALL}/bin/:$PATH

module load mpi/openmpi-4.1.4

# Activate Spack and environment
#source ~/spack/share/spack/setup-env.sh
#spack load ecp-proxy-apps@1.0
#spack load ecp-proxy-apps@4.0
#spack env activate ATS-5
#spack env activate ecp1.0
