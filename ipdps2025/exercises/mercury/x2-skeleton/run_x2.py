#!/usr/bin/env python
#
# Copyright 2009-2024 NTESS. Under the terms
# of Contract DE-NA0003525 with NTESS, the U.S.
# Government retains certain rights in this software.
#
# Copyright (c) 2009-2024, NTESS
# All rights reserved.
#
# This file is part of the SST software package. For license
# information, see the LICENSE file in the top level directory of the
# distribution.

import sst
from sst.merlin.base import *
from sst.merlin.endpoint import *
from sst.merlin.interface import *
from sst.merlin.topology import *
from sst.hg import *

if __name__ == "__main__":

    PlatformDefinition.loadPlatformFile("platform_file_hg_test")
    PlatformDefinition.setCurrentPlatform("platform_hg_test")
    platform = PlatformDefinition.getCurrentPlatform()

    platform.addParamSet("operating_system", {
        "verbose" : 0,
        "app1.name" : "x2",
        "app1.exe_library_name"  : "x2",
        "app1.verbose" : 0, 
        "app1.dependencies" : ["sumi",],
        "app1.libraries"    : ["systemlibrary:SystemLibrary",
                               "computelibrary:ComputeLibrary",
                               "mask_mpi:MpiApi",]
    })

    topo = topoFatTree()
    topo.shape = "4,4:4,4:8"

    ep = HgJob(0,2)

    system = System()
    system.setTopology(topo)
    system.allocateNodes(ep,"linear")

    system.build()
