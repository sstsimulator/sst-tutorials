// -*- mode: c++ -*-

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


#ifndef SST_EXTERNAL_COMPONENTS_ARCHIMEDES_TOPOLOGY_RING_H
#define SST_EXTERNAL_COMPONENTS_ARCHIMEDES_TOPOLOGY_RING_H

#include <sst/core/event.h>
#include <sst/core/params.h>

#include <string.h>

// Defininition of Topolgy and other base classes from merlin
#include "sst/elements/merlin/router.h"

// Put object into namespace to avoid naming conflicts
namespace Archimedes {

/**
   internal_router_event specialized for ring topology.  It stores the
   router number and the host_port for the destination of the event on
   the network.
 */
class topo_ring_event : public SST::Merlin::internal_router_event {
public:

    enum class Direction {Left, Right}; 

    int router;
    int host_port;
    Direction direction;

    
    topo_ring_event() {}
    topo_ring_event(int router, int port) : router(router), host_port(port) {}

    virtual ~topo_ring_event() {}

    // Clone the event.  This is used for events sent during init when
    // the broadcast address is used as the dest
    internal_router_event* clone(void) override
    {
        // Use default copy constructor to make a copy of the event
        // and return it
        topo_ring_event* tte = new topo_ring_event(*this);
        return tte;
    }

    // Serialize/deserialize the event to send between MPI ranks
    void serialize_order(SST::Core::Serialization::serializer &ser)  override {
        internal_router_event::serialize_order(ser);
        ser& router;
        ser& host_port;
        ser& direction;
    }

    // Call the serialization macro that will set up the functions needed to serialize t
    ImplementSerializable(Archimedes::topo_ring_event)
};



class topo_ring: public SST::Merlin::Topology {

public:

    SST_ELI_REGISTER_SUBCOMPONENT(
        topo_ring,
        "archimedes",
        "ring",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Ring topology object",
        SST::Merlin::Topology
    )

    SST_ELI_DOCUMENT_PARAMS(
        // Parameters needed for use with old merlin python module
        {"local_ports",  "Number of endpoints attached to each router.", "1"},
        {"interrouter_links", "Number of link between each router", "1"},
        {"num_routers", "Number of routers in the ring"}
    )


private:
    // Parameters passed through constructor
    int num_ports;
    int router_id;
    int num_vns;

    // Parameters found in the Params object
    int num_local_ports;
    int num_interrouter_links;
    int num_routers;

    // Internal variables pointing to start of various port blocks
    int local_port_start;
    int left_port_start;
    int right_port_start;

    inline bool _isLocalPort(int port) const {
        return (port >= local_port_start) && (port < (local_port_start + num_local_ports));
    }
    inline bool _isLeftPort(int port) const {
        return (port >= left_port_start) && (port < (left_port_start + num_interrouter_links));
    }
    inline bool _isRightPort(int port) const {
        return (port >= right_port_start) && (port < (right_port_start + num_interrouter_links));
    }
    
public:
    topo_ring(SST::ComponentId_t cid, SST::Params& params, int num_ports, int rtr_id, int num_vns);
    ~topo_ring() {}

    void route_packet(int port, int vc, SST::Merlin::internal_router_event* ev) override;
    SST::Merlin::internal_router_event* process_input(SST::Merlin::RtrEvent* ev) override;
    std::pair<int,int> getDeliveryPortForEndpointID(int ep_id) override;

    void routeUntimedData(int port, SST::Merlin::internal_router_event* ev, std::vector<int> &outPorts) override;
    SST::Merlin::internal_router_event* process_UntimedData_input(SST::Merlin::RtrEvent* ev) override;

    SST::Merlin::Topology::PortState getPortState(int port) const override;
    int getEndpointID(int port) override;

    void getVCsPerVN(std::vector<int>& vcs_per_vn) override;
    
};

}

#endif // SST_EXTERNAL_COMPONENTS_ARCHIMEDES_TOPOLOGY_RING_H
