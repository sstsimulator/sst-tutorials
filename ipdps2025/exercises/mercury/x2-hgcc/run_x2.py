#!/usr/bin/env python
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
        "app1.exe_library_name"  : "x2-hgcc",
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
