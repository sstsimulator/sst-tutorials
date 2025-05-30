#!/usr/bin/env python3
#
# Copyright (C) 2017-2024 Tactical Computing Laboratories, LLC
# All Rights Reserved
# contact@tactcomplabs.com
#
# See LICENSE in the top level directory for licensing details
#
# rev-test-pthread.py
#

import os
import sst
import sys
import argparse

# Setup argument parser
parser = argparse.ArgumentParser(description="Run Rev SST Simulation")
parser.add_argument("--numCores", type=int, help="Number of Rev Cores per RevCPU", default=1)
parser.add_argument("--numHarts", type=int, help="Number of Rev Harts per RevCPU", default=1)
parser.add_argument("--program", help="The program executable to run in the simulation", default="pthreads.exe")

# Parse arguments
args = parser.parse_args()

# Define SST core options
sst.setProgramOption("timebase", "1ps")

# Tell SST what statistics handling we want
sst.setStatisticLoadLevel(4)

# Define the simulation components
# Instantiate all the CPUs
for i in range(0, args.numCores):
    print("Building " + str(i))
    comp_cpu = sst.Component("cpu" + str(i), "revcpu.RevCPU")
    comp_cpu.addParams({
          "verbose": 5,                       # Verbosity
          "numCores": 1,                      # Number of cores
          "numHarts": args.numHarts,          # Number of harts
          "clock": "1.0GHz",                  # Clock
          "memSize": 1024*1024*1024,          # Memory size in bytes
          "machine": "[CORES:RV64GC]",        # Core:Config; common arch for all
          "startAddr": "[CORES:0x00000000]",  # Starting address for core 0
          "memCost": "[0:1:10]",              # Memory loads required 1-10 cycles
          "program": args.program,            # Target executable
          "splash": 0,                        # Display the splash message
          })
#  comp_cpu.enableAllStatistics()

# sst.setStatisticOutput("sst.statOutputCSV")
# sst.enableAllStatisticsForAllComponents()

# EOF
