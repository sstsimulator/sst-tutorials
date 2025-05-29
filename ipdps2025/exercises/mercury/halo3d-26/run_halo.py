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
        "verbose" : "0",
        "app1.name" : "halo3d26",
        "app1.exe_library_name" : "halo3d26",
        "app1.dependencies" : ["sumi", ],
        "app1.libraries" : ["computelibrary:ComputeLibrary",
                            "mask_mpi:MpiApi",],
    })

    topo = topoFatTree()
    topo.shape = "4,4:4,4:8"

    ep = HgJob(0,8)

    system = System()
    system.setTopology(topo)
    system.allocateNodes(ep,"linear")

    system.build()
