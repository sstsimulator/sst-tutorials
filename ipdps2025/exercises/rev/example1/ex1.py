#!/usr/bin/env python3
#
# Copyright (C) 2017-2024 Tactical Computing Laboratories, LLC
# All Rights Reserved
# contact@tactcomplabs.com
#
# See LICENSE in the top level directory for licensing details
#
# ex1.py
#

import os
import sst

DEBUG_L1 = 1
DEBUG_MEM = 10
DEBUG_LEVEL = 10
VERBOSE = 10
memSize = 1024 * 1024 * 1024 - 1

# Define the simulation components
comp_cpu = sst.Component("cpu", "revcpu.RevCPU")
comp_cpu.addParams(
    {
        "verbose": 4,                                    # Verbosity
        "numCores": 1,                                   # Number of cores
        "clock": "2.0GHz",                               # Clock
        "memSize": memSize,                              # Memory size in bytes
        "machine": "[0:RV32I]",                          # Core:Config; RV32I for core 0
        "startAddr": "[0:0x00000000]",                   # Starting address for core 0
        "memCost": "[0:1:10]",                           # Memory loads required 1-10 cycles
        "program": "ex1.exe",                            # Target executable
        "enableMemH": 0,                                 # Disable memHierarchy support
        "splash": 1,                                     # Display the splash message
    }
)
comp_cpu.enableAllStatistics()

# EOF
