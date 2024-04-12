// Copyright 2009-2024 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2023, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// of the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.
//
#include "topo_ring.h"

#include <sst/core/params.h>
#include <sst/core/eli/elibase.h>
#include <sst/core/eli/paramsInfo.h>
#include <sst/elements/merlin/router.h>

#include <sst/core/model/element_python.h>



// Set up helper python module
namespace Archimedes {

// Byte-ized Python file prepared for loading into the Python
// interpreter
char pyarchimedes[] = {
#include "pyarchimedes.inc"
    0x00};

// Class the holds the ELI information for loading the Archimedes
// Python module into the interpreter
class ArchimedesPyModule : public SST::SSTElementPythonModule {
public:
    ArchimedesPyModule(std::string library) :
        SSTElementPythonModule(library)
    {
        createPrimaryModule(pyarchimedes,"pyarchimedes.py");
    }

    SST_ELI_REGISTER_PYTHON_MODULE(
        Archimedes::ArchimedesPyModule,
        "archimedes",
        SST_ELI_ELEMENT_VERSION(1,0,0)
    )

    // Need to export the ELI since it's not contained in a header
    // file
    SST_ELI_EXPORT(Archimedes::ArchimedesPyModule)    
};
}

using namespace Archimedes;


// Methods for topo_ring
topo_ring::topo_ring(SST::ComponentId_t cid, SST::Params& params, int num_ports, int rtr_id, int num_vns) :
    SST::Merlin::Topology(cid),
    num_ports(num_ports),
    router_id(rtr_id),
    num_vns(num_vns)
{
    // Get the topology parameters from the Params object
    num_local_ports = params.find<int>("hosts_per_router", 1);

    num_interrouter_links = params.find<int>("interrouter_links", 1);

    num_routers = params.find<int>("num_routers", -1);
    if ( num_routers == -1 ) {
        getSimulationOutput().fatal(CALL_INFO_LONG, -1,
                                    "ERROR: topo_ring: must specify number of routers in the topology");
    }

    // Get start of port groups

    // Port map is local ports, left ports, right ports
    local_port_start = 0;
    left_port_start = local_port_start + num_interrouter_links;
    right_port_start = left_port_start + num_interrouter_links;
}

void
topo_ring::route_packet(int port, int vc, SST::Merlin::internal_router_event* ev)
{
    topo_ring_event* tre = static_cast<topo_ring_event*>(ev);
    // Compute the output port.

    // If we are at the correct router, just send it to the host_port
    if ( router_id == tre->router ) {
        ev->setNextPort(tre->host_port);
        return;
    }

    // If not at correct router, continue in the same direction as
    // before and set the base port for that direction
    int base_port = tre->direction == topo_ring_event::Direction::Right ? right_port_start : left_port_start;

    // Spread the traffic out over the interrouter links based on src+dest
    int next = base_port + ( (tre->getSrc() + tre->getDest() ) % num_interrouter_links );

    // Need to handle the dateline to avoid routing deadlock.  If we
    // are router 0 and packet came from another router, we are
    // crossing the dateline.
    // if ( router_id == 0 && port >= left_port_start ) {
    if ( router_id == 0 && !_isLocalPort(port) ) {
        int new_vc = vc ^ 1;
        tre->setVC(new_vc);
    }
    ev->setNextPort(next);
}

SST::Merlin::internal_router_event*
topo_ring::process_input(SST::Merlin::RtrEvent* ev)
{
    topo_ring_event* tre = new topo_ring_event();
    tre->setEncapsulatedEvent(ev);
    tre->setVC(tre->getVN() * 2);

    // Break down the locations in the ring based on the dest ID
    tre->router = ev->getDest() / num_local_ports;

    tre->host_port = (ev->getDest() % num_local_ports + local_port_start);

    // Figure out what direction to go in
    int dest_rtr = tre->router;
    int dest_right = dest_rtr > router_id ? (dest_rtr - router_id) : (dest_rtr + num_routers - router_id);
    int dest_left  = router_id > dest_rtr ? (router_id - dest_rtr) : (router_id + num_routers - dest_rtr);

    // Pick the distance, ties will go right (this would be better if
    // we had a tie-breaker that broke traffic into the two
    // directions)
    if ( dest_right <= dest_left ) tre->direction = topo_ring_event::Direction::Right;
    else tre->direction = topo_ring_event::Direction::Left;
    
    return tre;
}


std::pair<int,int>
topo_ring::getDeliveryPortForEndpointID(int ep_id) {
    return std::make_pair<int,int>(ep_id / num_local_ports,
                                   (ep_id % num_local_ports + local_port_start));
}


void
topo_ring::routeUntimedData(int port, SST::Merlin::internal_router_event* ev, std::vector<int> &outPorts) {
    topo_ring_event* tre = static_cast<topo_ring_event*>(ev);
    
    if ( tre->getDest() == SST::Merlin::UNTIMED_BROADCAST_ADDR ) {

        // Check to see if this is the injection point
        if ( _isLocalPort(port) ) {
            // Send a copy of the event to everyone but the orginal
            // sender, then set direction to Right and pass event
            // right to next router.
            for ( int i = local_port_start; i < (local_port_start + num_local_ports); ++i ) {
                if ( i != port )
                    outPorts.push_back(i);
            }
            // Send it right as well
            outPorts.push_back(right_port_start);
        }
        else {
            // If we aren't back at the beginning, send things to all
            // the ports and then to the right. If we are at the src
            // router, do nothing to end broadcast
            if ( router_id != tre->router ) {
                for ( int i = 0; i < num_local_ports; ++i ) {
                    outPorts.push_back(i + local_port_start);
                }
                // Send it right as well
                outPorts.push_back(right_port_start);
            }
        }
    }
    else {
        // Just call route_packet() and add next port to outPorts
        route_packet(port, 0, tre);
        outPorts.push_back(tre->getNextPort());
    }
}

SST::Merlin::internal_router_event*
topo_ring::process_UntimedData_input(SST::Merlin::RtrEvent* ev)
{
    if ( ev->getDest() == SST::Merlin::UNTIMED_BROADCAST_ADDR) {
        topo_ring_event* tre = new topo_ring_event();
        tre->setEncapsulatedEvent(ev);
        tre->router = router_id;
        tre->direction = topo_ring_event::Direction::Left;
        return tre;
    }
    else {
        // Just call process_input
        return process_input(ev);
    }
}

SST::Merlin::Topology::PortState
topo_ring::getPortState(int port) const
{
    if ( _isLocalPort(port) ) return R2N;
    if ( _isLeftPort(port) || _isRightPort(port) ) return R2R;
    return UNCONNECTED;
}



int
topo_ring::getEndpointID(int port)
{
    if ( !_isLocalPort(port) ) return -1;
    return (router_id * num_local_ports) + port - local_port_start;
}

void
topo_ring::getVCsPerVN(std::vector<int>& vcs_per_vn)
{
    for ( int i = 0; i < num_vns; ++i ) {
        // Need 2 VCs per VN so we can change VCs at dateline to
        // avoid routing loops
        vcs_per_vn[i] = 2;
    }
}


