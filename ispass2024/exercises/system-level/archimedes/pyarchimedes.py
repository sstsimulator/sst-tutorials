#!/usr/bin/env python
#
# Copyright 2009-2024 NTESS. Under the terms
# of Contract DE-NA0003525 with NTESS, the U.S.
# Government retains certain rights in this software.
#
# Copyright (c) 2009-2024, NTESS
# All rights reserved.
#
# Portions are copyright of other developers:
# See the file CONTRIBUTORS.TXT in the top level directory
# of the distribution for more information.
#
# This file is part of the SST software package. For license
# information, see the LICENSE file in the top level directory of the
# distribution.

import sst
from sst.merlin.base import *

# Class to do most of the work for both mesh and torus
class topoRing(Topology):
    def __init__(self):
        Topology.__init__(self)
        self._declareClassVariables(["link_latency","host_link_latency","bundleEndpoints"])
        self._declareParams("main",["hosts_per_router", "interrouter_links", "num_routers"])
        self._subscribeToPlatformParamSet("topology")


    def getName():
        return "Ring"
        

    def getNumNodes(self):
        return self.num_routers * self.hosts_per_router


    def getRouterNameForId(self,rtr_id):
        return "rtr_%s"%rtr_id
        
    def _build_impl(self, endpoint):
        if self.host_link_latency is None:
            self.host_link_latency = self.link_latency
        
        # get some local variables from the parameters
        local_ports = int(self.hosts_per_router)
        num_routers = int(self.num_routers)
        num_peers = num_routers * local_ports

        radix = local_ports + (2 * int(self.interrouter_links))

        links = dict()

        #####################
        def getLink(rtr0, rtr1, rail):
            if rtr0 > rtr1:
                high = rtr0
                low = rtr1
            else:
                high = rtr1
                low = rtr0

            name = "link_%d_%d_%d"%(low,high,rail)    
            if name not in links:
                links[name] = sst.Link(name)

            return links[name]
        #####################
        
        for router_num in range(num_routers):
            rtr = self._instanceRouter(radix, router_num)
            
            topology = rtr.setSubComponent(self.router.getTopologySlotName(),"archimedes.ring")
            self._applyStatisticsSettings(topology)
            topology.addParams(self._getGroupParams("main"))

            # Instance the endpoints
            for port in range(local_ports):
                nodeID = local_ports * router_num + port
                (ep, port_name) = endpoint.build(nodeID, {})
                if ep:
                    nicLink = sst.Link("nic%d"%(nodeID))
                    if self.bundleEndpoints:
                       nicLink.setNoCut()
                    nicLink.connect( (ep, port_name, self.host_link_latency), (rtr, "port%d"%port, self.host_link_latency) )

            # Connect the left port(s)
            left = router_num - 1
            if ( left < 0 ):
                left = num_routers - 1

            port_base = self.hosts_per_router
            for port_offset in range(self.interrouter_links):
                rtr.addLink(getLink(router_num, left, port_offset), "port%d"%(port_base + port_offset), self.link_latency)

            # Connect the right port(s)
            right = router_num + 1
            if ( right >= num_routers ):
                right = 0

            port_base = self.hosts_per_router + self.interrouter_links
            for port_offset in range(self.interrouter_links):
                rtr.addLink(getLink(router_num, right, port_offset), "port%d"%(port_base + port_offset), self.link_latency)


