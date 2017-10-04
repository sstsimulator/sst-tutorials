import sst
import os
import ConfigParser
from sst import merlin



def connect(name, c0, port0, c1, port1, latency):
    link = sst.Link(name)
    link.connect( (c0, port0, latency), (c1, port1, latency) )
    return link




class CPUConfig:
    def __init__(self, cp):
        self.cfg = dict(cp.items('CPU'))
        self.app = self.cfg['application']
        self.applicationParams = dict(cp.items(self.app))
        self.total_cores = 0
        self.next_id = 0

        if self.app == 'miranda.STREAMBenchGenerator':
            self.coreConfig = self._streamCoreConfig
        else:
            raise Exception("Unknown application '%s'"%app)

    def updateTotalCores(self, nCores):
        self.total_cores = nCores


    def _streamCoreConfig(self, core_id):
        streamN = int(self.applicationParams['total_streamn'])
        params = dict()
        params['max_reqs_cycle'] =  self.cfg['max_reqs_cycle']
        params['generator'] = 'miranda.STREAMBenchGenerator'
        params['generatorParams.n'] = streamN / self.total_cores
        params['generatorParams.start_a'] = ( (streamN * 32) / self.total_cores ) * core_id
        params['generatorParams.start_b'] = ( (streamN * 32) / self.total_cores ) * core_id + (streamN * 32)
        params['generatorParams.start_c'] = ( (streamN * 32) / self.total_cores ) * core_id + (2 * streamN * 32)
        params['generatorParams.operandwidth'] = 32
        #params['generatorParams.verbose'] = int(self.verbose)
        return params

    def build(self, nID, netCfg):
        cpu = sst.Component("cpu%d"%(self.next_id), "miranda.BaseCPU")
        cpu.addParams(self.coreConfig(self.next_id))
        cpu.addParam("clock", self.cfg["clock"])
        cpuPort = "cache_link"

        l1 = sst.Component("l1cache_%d"%(self.next_id), "memHierarchy.Cache")
        l1.addParams(dict({
            "prefetcher": "cassini.StridePrefetcher",
            "prefetcher.reach": 4,
            "prefetcher.detect_range" : 1,
            "cache_frequency": self.cfg["clock"],
            "cache_size": "32KB",
            "associativity": 8,
            "access_latency_cycles": 4,
            "L1": 1,
            # Default params
            # "cache_line_size": 64,
            # "coherence_protocol": self.coherence_protocol,
            # "replacement_policy": "lru",
            # Not neccessary for simple cases:
            #"maxRequestDelay" : "1000000",
            }))

        l2 = sst.Component("l2cache_%d"%(self.next_id), "memHierarchy.Cache")
        l2.addParams(dict({
            "prefetcher": "cassini.StridePrefetcher",
            "prefetcher.reach": 16,
            "prefetcher.detect_range" : 1,
            "cache_frequency": self.cfg["clock"],
            "cache_size": "256KB",
            "associativity": 8,
            "access_latency_cycles": 6,
            "mshr_num_entries" : 16,
            "network_bw": netCfg.cfg["bandwidth"],
            # Default params
            #"cache_line_size": 64,
            #"coherence_protocol": self.coherence_protocol,
            #"replacement_policy": "lru",
            "network_address": nID 
            }))

        connect("cpu_cache_link_%d"%self.next_id,
                cpu, cpuPort,
                l1, "high_network_0",
                netCfg.cfg["latency"]).setNoCut()

        connect("l2cache_%d_link"%self.next_id,
                l1, "low_network_0",
                l2, "high_network_0",
                netCfg.cfg["latency"]).setNoCut()

        self.next_id += 1

        return (l2, "cache", netCfg.cfg["latency"])


class MemConfig:
    def __init__(self, cp):
        self.cfg = dict(cp.items('Memory'))
        self.next_id = 0
    def updateTotalMems(self, nMC):
        self.num_MC = nMC

    def build(self, nID, netCfg):
        mem = sst.Component("memory_%d"%(self.next_id), "memHierarchy.MemController")
        mem.addParams(dict({
            "backend" : "memHierarchy.simpleMem",
            "backend.access_time" : "30ns",
            "backend.mem_size" : "%d%s"%(int(filter(str.isdigit, self.cfg["capacity"])) / (self.num_MC), filter(str.isalpha, self.cfg["capacity"])),
            "clock" : self.cfg["clock"],
            "network_bw": self.cfg["network_bw"],
            "do_not_back" : 1,
            }))

        dc = sst.Component("dc_%d"%(self.next_id), "memHierarchy.DirectoryController")
        dc.addParams(dict({
            "memNIC.interleave_size": "%dB"%int(self.cfg["interleave_size"]),
            "memNIC.interleave_step": "%dB"%(self.num_MC * int(self.cfg["interleave_size"])),
            "entry_cache_size": 256*1024*1024, #Entry cache size of mem/blocksize
            "clock": self.cfg["clock"],
            "memNIC.network_bw": self.cfg["network_bw"],
            "memNIC.addr_range_start" : self.next_id * int(self.cfg["interleave_size"]),
            "memNIC.addr_range_end" : (int(filter(str.isdigit, self.cfg["capacity"])) * 1024 * 1024) - (self.num_MC * int(self.cfg["interleave_size"])) + (self.next_id * int(self.cfg["interleave_size"])),
            "network_address": nID
            }))

        connect("mem_link_%d"%self.next_id,
                mem, "direct_link",
                dc, "memory",
                netCfg.cfg["latency"])

        self.next_id += 1

        return (dc, "network", netCfg.cfg["latency"])



class NetConfig:
    class TopoConfig:
        def getMerlinGenerator(self):
            pass
        def getMerlinParams(self):
            return dict()
        def numEndPoints(self):
            0

    class MeshTopologyConfig(TopoConfig):
        def __init__(self, params):
            self.params = params
            self.layout = params['layout'].split('x')
        def numEndPoints(self):
            return reduce(lambda x,y: x*y, [int(x) for x in self.layout])
        def getMerlinGenerator(self):
            return merlin.topoMesh()
        def getMerlinParams(self):
            return dict({
                "num_ports": 1 + 2*len(self.layout),
                "topology": "mesh",
                "num_dims": len(self.layout),
                "mesh:width": "x".join(["1" for x in range(len(self.layout))]),
                "mesh:shape": "x".join(self.layout),
                "mesh:local_ports": 1
            })



    def __init__(self, cp, cfgGroup='Network', cfgPrefix=''):
        self.cfg = { key[len(cfgPrefix):]: value
                    for (key, value) in dict(cp.items(cfgGroup)).iteritems()
                    if key[:len(cfgPrefix)] == cfgPrefix }
        if "topology" in self.cfg:
            self.topo = self.buildTopo()
        else:
            self.topo = None

    def buildTopo(self):
        topo_type = self.cfg["topology"] + "_"
        topoParams = { key[len(topo_type):]: value
                    for (key, value) in self.cfg.iteritems()
                    if key[:len(topo_type)] == topo_type }

        if topo_type == "mesh_":
            return self.MeshTopologyConfig(topoParams)
        else:
            raise Exception("Unknown topology type '%s'"%topo_type)

    def endPointCount(self):
        return self.topo.numEndPoints()
            
    def getMerlinGenerator(self):
        return self.topo.getMerlinGenerator()
    def getMerlinParams(self):
        p = dict({
            "output_latency": "25ps",
            "output_buf_size" : "2KB",
            "input_latency" : "25ps",
            "input_buf_size" : "2KB",
            "flit_size": self.cfg["flit_size"],
            "xbar_bw": self.cfg["bandwidth"],
            "link_bw": self.cfg["bandwidth"],
            "link_lat": self.cfg["latency"],
        })
        # Missing:   num_ports, topology, num_dims
        if self.topo:
            p.update(self.topo.getMerlinParams())
        return p



class GroupConfig:
    def __init__(self, cp):
        self.cfg = dict(cp.items('Groups'))
        self.cpuConfig = CPUConfig(cp)
        self.memConfig = MemConfig(cp)
        self.netCfg = NetConfig(cp, cfgGroup='Groups', cfgPrefix='net_')

        self.next_l3_id = 0

    def setNumGroups(self, numGroups):
        self.num_groups = numGroups
        self.cpuConfig.updateTotalCores(numGroups * int(self.cfg['cores']))
        self.memConfig.updateTotalMems(numGroups * int(self.cfg['memory_controllers']))

    def getNumCores(self):
        return int(self.cfg['cores'])

    def buildL3(self, nID, netCfg):
        l3cache = sst.Component("l3cache_%d"%(self.next_l3_id), "memHierarchy.Cache")
        l3cache.addParams(dict({
            "access_latency_cycles" : "12",
            "cache_frequency" : self.cfg["clock"],
            "associativity" : "16",
            "cache_size" : self.cfg["l3cache_block_size"],
            "mshr_num_entries" : "4096",
            "network_bw": netCfg.cfg["bandwidth"],
            # Distributed caches
            "num_cache_slices" : self.num_groups * int(self.cfg["l3cache_blocks"]),
            "slice_allocation_policy" : "rr",
            # Default params
            # "replacement_policy" : "lru",
            # "cache_line_size" : "64",
            # "coherence_protocol" : coherence_protocol,
            "network_address" : nID,
            "slice_id": self.next_l3_id
            }))

        self.next_l3_id += 1

        return (l3cache, "directory", netCfg.cfg["latency"])

    def buildGroup(self, groupID, bridge):
        print "Building group", groupID
        prefix = "Link:Group%d:"%groupID
        # Build rtr
        rtr = sst.Component("local_rtr", "merlin.hr_router")
        rtr.addParams(self.netCfg.getMerlinParams())
        rtr.addParams(dict({
            "num_ports": 1 +
                int(self.cfg["cores"]) + 
                int(self.cfg["l3cache_blocks"]) + 
                int(self.cfg["memory_controllers"]),
            "topology": "merlin.singlerouter",
            "id": 0
            }))
        # Connect to bridge
        sst.Link(prefix+"bridgeRtr").connect(
            (rtr, "port0", self.netCfg.cfg["latency"]),
                (bridge, "network0", self.netCfg.cfg["latency"]))
        # Connect CPUs
        port = 1
        for coreID in range(int(self.cfg["cores"])):
            sst.Link(prefix+"Core%d"%coreID).connect(
                    (rtr, "port%d"%port, self.netCfg.cfg["latency"]),
                    self.cpuConfig.build(port, self.netCfg))
            port += 1

        # Connect L3s
        for l3ID in range(int(self.cfg["l3cache_blocks"])):
            sst.Link(prefix+"L3_%d"%l3ID).connect(
                    (rtr, "port%d"%port, self.netCfg.cfg["latency"]),
                    self.buildL3(port, self.netCfg))
            port += 1

        # Connect Memories
        for memID in range(int(self.cfg["memory_controllers"])):
            sst.Link(prefix+"MEM_%d"%memID).connect(
                    (rtr, "port%d"%port, self.netCfg.cfg["latency"]),
                    self.memConfig.build(port, self.netCfg))
            port += 1


    def build(self, nID):
        sst.pushNamePrefix("group%d"%nID)
        bridge = sst.Component("bridge", "merlin.Bridge")
        bridge.addParams({
            "translator": "memHierarchy.MemNetBridge",
            "debug": 0,
            "debug_level" : 0,
            "network_bw" : self.netCfg.cfg["bandwidth"],
            })
        self.buildGroup(nID, bridge)
        ret = (bridge, "network1", self.netCfg.cfg["latency"])
        sst.popNamePrefix()
        return ret


class EndPointGenerator(merlin.EndPoint):
    def __init__(self, groupConfig):
        #super(EndPointGenerator, self).__init__()
        sst.merlin.EndPoint.__init__(self)
        self.cfg = groupConfig
    def getName(self):
        return "Example Endpoint Generator"
    def build(self, nID, extraKeys):
        return self.cfg.build(nID)

class ChipConfig:

    def __init__(self, cfgFile, **kwargs):
        cp = ConfigParser.ConfigParser()
        if not cp.read(cfgFile):
            raise Exception('Unable to read file "%s"'%cfgFile)

        self.verbose = "verbose" in kwargs and kwargs["verbose"]

        self.groupConfig = GroupConfig(cp)
        self.netConfig = NetConfig(cp)
        self.groupConfig.setNumGroups(self.netConfig.endPointCount())

    def build(self):
        topoGen = self.netConfig.getMerlinGenerator()
        # Hacky way to pass parameters to the Topology Builder
        sst.merlin._params.update(self.netConfig.getMerlinParams())
        topoGen.prepParams()
        topoGen.setEndPoint(EndPointGenerator(self.groupConfig))
        topoGen.build()




