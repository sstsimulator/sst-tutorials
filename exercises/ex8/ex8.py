import sst
import sys
import ConfigParser, argparse
from utils import *
from sst import merlin


# Parse commandline arguments
parser = argparse.ArgumentParser()
parser.add_argument("-c", "--config", help="specify configuration file", required=True)
parser.add_argument("-v", "--verbose", help="increase verbosity of output", action="store_true")
parser.add_argument("-s", "--statfile", help="statistics file", default="./stats.csv")
parser.add_argument("-l", "--statlevel", help="statistics level", type=int, default=16)

args = parser.parse_args()

verbose = args.verbose
cfgFile = args.config
statFile = args.statfile
statLevel = args.statlevel

# Build Configuration Information
config = SNBConfig(cfgFile, verbose=verbose)


if 'ariel' in config.app:
    arielCPU = sst.Component("A0", "ariel.ariel")
    arielCPU.addParams(config.getCoreConfig(0))



class EndpointCreator(merlin.EndPoint):
    _CORE = "CORE"
    _L3 = "L3"
    _MEMORY = "MEMORY"
    class GroupInfo():
        def __init__(self, num):
            self.groupNum = num 
            self.coreCount = 0
            self.L3Count = 0
            self.MemCCount = 0

        def isDone(self, config):
            return ((self.coreCount == config.cores_per_group) and
                    (self.L3Count == config.l3cache_blocks_per_group) and
                    (self.MemCCount == config.memory_controllers_per_group))

        def nextItem(self, config):
            if self.coreCount < config.cores_per_group:
                if self.L3Count < config.l3_cache_per_core * (self.coreCount + 1):
                    self.L3Count += 1
                    return EndpointCreator._L3
                else:
                    self.coreCount += 1
                    return EndpointCreator._CORE
            elif self.L3Count < config.l3cache_blocks_per_group:
                self.L3Count += 1
                return EndpointCreator._L3
            else:
                self.MemCCount += 1
                return EndpointCreator._MEMORY


    def __init__(self, config):
        self.config = config;
        self.next_group_id = 0
        self.next_core_id = 0
        self.next_l3_cache_id = 0
        self.next_memory_ctrl_id = 0
        self.groupInfo = self.GroupInfo(0)
        merlin.EndPoint.__init__(self)

    def getName(self):
        return "Example Endpoint Generator"

    def build(self, nID, extraKeys):
        if self.groupInfo.isDone(self.config):
            self.next_group_id += 1
            self.groupInfo = self.GroupInfo(self.next_group_id)

        sst.pushNamePrefix("g%d"%self.groupInfo.groupNum)

        nodeType = self.groupInfo.nextItem(self.config)
        if nodeType == EndpointCreator._CORE:
            ret = self.buildCore(nID)
        elif nodeType == EndpointCreator._L3:
            ret = self.buildL3(nID)
        elif nodeType == EndpointCreator._MEMORY:
            ret = self.buildMemory(nID)
        else:
            print "Unknonw Next item type:  ", nodeType
            sst.exit(1)

        sst.popNamePrefix()
        return ret


    def buildCore(self, nID):
        local_core_id = self.next_core_id % self.config.cores_per_group
        print "Creating Core %d in Group %d"%(local_core_id, self.next_group_id)

        if 'miranda' in config.app:
            cpu = sst.Component("cpu%d"%(self.next_core_id), "miranda.BaseCPU")
            cpu.addParams(self.config.getCoreConfig(self.next_core_id))
            cpuPort = "cache_link"
        elif 'ariel' in config.app:
            cpu = arielCPU
            cpuPort = "cache_link_%d"%self.next_core_id

        l1 = sst.Component("l1cache_%d"%(self.next_core_id), "memHierarchy.Cache")
        l1.addParams(config.getL1Params())

        l2 = sst.Component("l2cache_%d"%(self.next_core_id), "memHierarchy.Cache")
        l2.addParams(config.getL2Params())
        l2.addParam( "network_address", nID )

        connect("cpu_cache_link_%d"%self.next_core_id,
                cpu, cpuPort,
                l1, "high_network_0",
                self.config.ring_latency).setNoCut()

        connect("l2cache_%d_link"%self.next_core_id,
                l1, "low_network_0",
                l2, "high_network_0",
                self.config.ring_latency).setNoCut()

        self.next_core_id += 1

        return (l2, "cache", self.config.ring_latency)

    def buildL3(self, nID):
        l3cache = sst.Component("l3cache_%d"%(self.next_l3_cache_id), "memHierarchy.Cache")
        l3cache.addParams(config.getL3Params())

        l3cache.addParams({
            "network_address" : nID,
            "slice_id" : self.next_l3_cache_id
            })
        self.next_l3_cache_id += 1

        return (l3cache, "directory", self.config.ring_latency)

    def buildMemory(self, nID):
        mem = sst.Component("memory_%d"%(self.next_memory_ctrl_id), "memHierarchy.MemController")
        mem.addParams(self.config.getMemParams())

        dc = sst.Component("dc_%d"%(self.next_memory_ctrl_id), "memHierarchy.DirectoryController")
        dc.addParams(self.config.getDCParams(self.next_memory_ctrl_id))
        dc.addParam("network_address", nID)

        connect("mem_link_%d"%self.next_memory_ctrl_id,
                mem, "direct_link",
                dc, "memory",
                self.config.ring_latency)

        self.next_memory_ctrl_id += 1

        return (dc, "network", self.config.ring_latency)



print "Configuring Ring Network-on-Chip..."

# Hacky way to pass parameters into the Topology Builder:
sst.merlin._params["num_dims"] = 1
sst.merlin._params["link_lat"] = config.ring_latency
sst.merlin._params.update(config.getRingParams())




topoGen = merlin.topoTorus()
topoGen.prepParams()
topoGen.setEndPoint(EndpointCreator(config))
topoGen.build()


# ===============================================================================

# Enable SST Statistics Outputs for this simulation
sst.setStatisticLoadLevel(statLevel)
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic"})

sst.setStatisticOutput("sst.statOutputCSV")
sst.setStatisticOutputOptions( {
    "filepath"  : statFile,
    "separator" : ", "
    } )

print "Completed configuring the SST Sandy Bridge model"
