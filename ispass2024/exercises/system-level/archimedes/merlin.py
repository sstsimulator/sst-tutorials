# Import the SST module
import sst

num_messages=8096
message_size=16

n_modules_per_group = 8
n_endpoints_per_module = 16
n_sockets_per_endpoint = 8
n_sockets_per_module = n_sockets_per_endpoint * n_endpoints_per_module
n_sockets_per_group = n_sockets_per_module * n_modules_per_group

n_routers_per_endpoint = 1
n_routers_per_module = 1
n_routers_per_group = 16

n_total_routers_per_endpoint = n_routers_per_endpoint
n_total_routers_per_module = n_total_routers_per_endpoint * n_endpoints_per_module + n_routers_per_module
n_total_routers_per_group = n_total_routers_per_module * n_modules_per_group + n_routers_per_group

n_groups=1
n_modules=1
n_endpoints=2
n_peers=n_endpoints*n_sockets_per_endpoint

def make_socket(num_peers, group_id, module_id, endpoint_id, socket_id):
    id = group_id * n_sockets_per_group + module_id * n_sockets_per_module + endpoint_id * n_sockets_per_endpoint + socket_id
    socket = sst.Component(f'socket{id}', 'merlin.test_nic')
    socket.addParam('id', id)
    socket.addParam('num_peers', num_peers)
    socket.addParam('num_messages', num_messages)
    socket.addParam('message_size', message_size)
    return socket

def make_iface(socket):
    iface = socket.setSubComponent('networkIF', 'merlin.linkcontrol', 0)
    iface.addParam('link_bw', '400Gb/s')
    iface.addParam('input_buf_size', '4kB')
    iface.addParam('output_buf_size', '4kB')
    return iface

def make_router(name, id):
    router = sst.Component(name, 'merlin.hr_router')
    router.addParam('id', id)
    router.addParam('num_vns', 1)
    router.addParam('flit_size', "64 b")
    router.addParam('input_latency', '20ns')
    router.addParam('output_latency', '20ns')
    router.addParam('input_buf_size', '4kB')
    router.addParam('output_buf_size', '4kB')
    router.addParam('link_bw', "800Gb/s")
    #router.addParam('xbar_arb', 'pilot_cris_elements.crossbar')
    return router

def make_endpoint(name, group_id, module_id, endpoint_id):
    id = group_id * n_total_routers_per_group + module_id * n_total_routers_per_module + endpoint_id * n_total_routers_per_endpoint
    ep = make_router(name, id)
    ep.addParam('num_ports', 9)
    ep.addParam('xbar_bw', "3600Gb/s")
    #ep.addParam('xbar_bw', "1000Gb/s")
    ep.addParam('link_bw:downstream', "400Gb/s")
    ep.addParam('link_bw:upstream', "3200Gb/s")

    topo = ep.setSubComponent('topology', 'pce.topology', 0)
    topo.addParam("n_groups", n_groups)
    topo.addParam("group_id", group_id)
    topo.addParam("module_id", module_id)
    topo.addParam("endpoint_id", endpoint_id)
    for i in range(0, 8):
        nic = make_socket(n_peers, group_id, module_id, endpoint_id, i)
        iface = make_iface(nic)
        link = sst.Link(f'rtr{id}_port{i}')
        #iface.addLink(link, 'rtr_port', '1ms')
        #ep.addLink(link, f'port{i}', '1ms')
        iface.addLink(link, 'rtr_port', '10ns')
        ep.addLink(link, f'port{i}', '10ns')
    return ep

ep0 = make_endpoint('ep0', 0, 0, 0)
ep1 = make_endpoint('ep1', 0, 0, 1)
link = sst.Link(f'link_ep0_ep1')
#ep0.addLink(link, 'port8', '1ms')
#ep1.addLink(link, 'port8', '1ms')
ep0.addLink(link, 'port8', '20ns')
ep1.addLink(link, 'port8', '20ns')

