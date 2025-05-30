#!/usr/bin/env python3
#
# Copyright (C) 2017-2024 Tactical Computing Laboratories, LLC
# All Rights Reserved
# contact@tactcomplabs.com
#
# See LICENSE in the top level directory for licensing details
#
# many_core_dist.py
#

import argparse
import os
import sst

DEBUG_L1 = 1
DEBUG_MEM = 10
DEBUG_LEVEL = 10
VERBOSE = 10
memSize = 1024 * 1024 * 1024 - 1

# Setup argument parser
parser = argparse.ArgumentParser(description="Run Rev SST Simulation")
parser.add_argument("--numCores", type=int, help="Number of Rev Cores per RevCPU", default=1)
parser.add_argument("--program", help="The program executable to run in the simulation", default="many_core.exe")

# Parse arguments
args = parser.parse_args()

# Define SST core options
sst.setProgramOption("timebase", "1ps")

# Tell SST what statistics handling we want
sst.setStatisticLoadLevel(4)

max_addr_gb = 1

# Define the simulation components

# -- Create the L2 cache and backing store
l2cache = sst.Component("l2cache", "memHierarchy.Cache")
l2cache.addParams(
    {
        "access_latency_cycles": "4",
        "cache_frequency": "2 Ghz",
        "replacement_policy": "lru",
        "coherence_protocol": "MESI",
        "associativity": "4",
        "cache_line_size": "64",
        "debug": 1,
        "debug_level": DEBUG_LEVEL,
        "verbose": VERBOSE,
        "L1": "0",
        "cache_size": "64KiB",
    }
)

bus = sst.Component("bus", "memHierarchy.Bus")
bus.addParams({
      "bus_frequency" : "2 Ghz",
})

memctrl = sst.Component("memory", "memHierarchy.MemController")
memctrl.addParams(
    {
        "debug": DEBUG_MEM,
        "debug_level": DEBUG_LEVEL,
        "clock": "2GHz",
        "verbose": VERBOSE,
        "addr_range_start": 0,
        "addr_range_end": memSize,
        "backing": "malloc",
    }
)

memory = memctrl.setSubComponent("backend", "memHierarchy.simpleMem")
memory.addParams({"access_time": "100ns", "mem_size": "8GB"})

link1 = sst.Link("bus_l2cache")
link1.connect((bus, "lowlink0", "1ns"), (l2cache, "highlink", "1ns"))
link2 = sst.Link("link2")
link2.connect((l2cache, "lowlink", "1ns"), (memctrl, "direct_link", "1ns"))


# -- Create the per-core CPU and L1 cache
for k in range(args.numCores):
    comp_cpu = sst.Component("cpu"+str(k), "revcpu.RevCPU")
    comp_cpu.addParams({
            "verbose": 4,                                      # Verbosity
            "numCores": 1,                                     # Number of cores
            "clock": "1.0GHz",                                 # Clock
            "memSize": 1024*1024*1024,                         # Memory size in bytes
            "machine": "[CORES:RV64GC]",                       # Core:Config; RV64I for core 0
            "startAddr": "[0:0x00000000]",                     # Starting address for core 0
            "memCost": "[0:1:10]",                             # Memory loads required 1-10 cycles
            "program": args.program,                           # Target executable
            "enableMemH": 1,                                   # Enable memHierarchy support
            "splash": 1                                        # Display the splash message
    })

    comp_cpu.enableAllStatistics()

    # Create the RevMemCtrl subcomponent
    comp_lsq = comp_cpu.setSubComponent("memory", "revcpu.RevBasicMemCtrl")
    comp_lsq.addParams(
        {
            "verbose": "10",
            "clock": "2.0Ghz",
            "max_loads": 64,
            "max_stores": 64,
            "max_flush": 64,
            "max_llsc": 64,
            "max_readlock": 64,
            "max_writeunlock": 64,
            "max_custom": 64,
            "ops_per_cycle": 64,
        }
    )
    comp_lsq.enableAllStatistics({"type": "sst.AccumulatorStatistic"})

    iface = comp_lsq.setSubComponent("memIface", "memHierarchy.standardInterface")
    iface.addParams({"verbose": VERBOSE})


    l1cache = sst.Component("l1cache"+str(k), "memHierarchy.Cache")
    l1cache.addParams(
        {
            "access_latency_cycles": "4",
            "cache_frequency": "2 Ghz",
            "replacement_policy": "lru",
            "coherence_protocol": "MESI",
            "associativity": "4",
            "cache_line_size": "64",
            "debug": 1,
            "debug_level": DEBUG_LEVEL,
            "verbose": VERBOSE,
            "L1": "1",
            "cache_size": "16KiB",
        }
    )

    link1 = sst.Link("cpu"+str(k)+"_l1cache")
    link1.connect((iface, "port", "1ns"), (l1cache, "highlink", "1ns"))
    link2 = sst.Link("l1cache"+str(k)+"_bus")
    link2.connect((l1cache, "lowlink", "1ns"), (bus, "highlink"+str(k), "1ns"))

# EOF
