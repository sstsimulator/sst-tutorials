#!/usr/bin/env python
#
# Copyright 2009-2023 NTESS. Under the terms
# of Contract DE-NA0003525 with NTESS, the U.S.
# Government retains certain rights in this software.
#
# Copyright (c) 2009-2023, NTESS
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
from sst.archimedes import *

if __name__ == "__main__":


    ### Setup the topology
    topo = topoRing()
    topo.hosts_per_router = 4
    topo.num_routers = 4
    topo.interrouter_links = 4

    # Set up the routers
    router = hr_router()
    router.link_bw = "4GB/s"
    router.flit_size = "8B"
    router.xbar_bw = "6GB/s"
    router.input_latency = "20ns"
    router.output_latency = "20ns"
    router.input_buf_size = "4kB"
    router.output_buf_size = "4kB"
    router.num_vns = 1
    router.xbar_arb = "merlin.xbar_arb_lru"

    # Add router template to topology and set link_latency
    topo.router = router
    topo.link_latency = "20ns"
    
    ### set up the endpoint
    networkif = LinkControl()
    networkif.link_bw = "4GB/s"
    networkif.input_buf_size = "4kB"
    networkif.output_buf_size = "4kB"

    ep = TestJob(0, topo.getNumNodes())
    ep.network_interface = networkif
    ep.num_messages = 10
    ep.message_size = "8B"
    ep.send_untimed_bcast = True
        
    system = System()
    system.setTopology(topo)
    system.allocateNodes(ep,"linear")

    system.build()
    

    # sst.setStatisticLoadLevel(9)

    # sst.setStatisticOutput("sst.statOutputCSV");
    # sst.setStatisticOutputOptions({
    #     "filepath" : "stats.csv",
    #     "separator" : ", "
    # })

