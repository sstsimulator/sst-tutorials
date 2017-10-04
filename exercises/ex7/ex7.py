import sst
import sys
import ConfigParser, argparse
from utils import *


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


router_map = {}
next_core_id = 0
next_network_id = 0
next_memory_ctrl_id = 0
next_l3_cache_id = 0


if 'ariel' in config.app:
    arielCPU = sst.Component("A0", "ariel.ariel")
    arielCPU.addParams(config.getCoreConfig(0))

print "Configuring Ring Network-on-Chip..."

for next_ring_stop in range(config.num_ring_stops):
    ring_rtr = sst.Component("rtr.%d"%next_ring_stop, "merlin.hr_router")
    ring_rtr.addParams(config.getRingParams())
    ring_rtr.addParam( "id", next_ring_stop)
    router_map["rtr.%d"%next_ring_stop] = ring_rtr

for next_ring_stop in range(config.num_ring_stops):
    if next_ring_stop == 0:
        # First stop
        connect("rtr_pos_%d"%next_ring_stop,
                router_map["rtr.0"], "port0",
                router_map["rtr.1"], "port1", config.ring_latency)
        connect("rtr_neg_%d"%next_ring_stop,
                router_map["rtr.0"], "port1",
                router_map["rtr.%d"%(config.num_ring_stops-1)], "port0", config.ring_latency)
    elif next_ring_stop == (config.num_ring_stops) - 1:
        # Last Stop
        connect("rtr_pos_%d"%next_ring_stop,
                router_map["rtr.%d"%next_ring_stop], "port0",
                router_map["rtr.0"], "port1", config.ring_latency)
        connect("rtr_neg_%d"%next_ring_stop,
                router_map["rtr.%d"%next_ring_stop], "port1",
                router_map["rtr.%d"%(next_ring_stop-1)], "port0", config.ring_latency)
    else:
        # Middle stops
        connect("rtr_pos_%d"%next_ring_stop,
                router_map["rtr.%d"%next_ring_stop], "port0",
                router_map["rtr.%d"%(next_ring_stop+1)], "port1", config.ring_latency)
        connect("rtr_neg_%d"%next_ring_stop,
                router_map["rtr.%d"%next_ring_stop], "port1",
                router_map["rtr.%d"%(next_ring_stop-1)], "port0", config.ring_latency)

for next_group in range(config.groups):
    print "Configuring core and memory controller group %d..."%next_group

    sst.pushNamePrefix("g%d"%next_group)

    # Connect Cores & caches
    for next_active_core in range(config.cores_per_group):
        # Connect L3 cache blocks to ring
        for next_l3_cache_block in range(config.l3_cache_per_core):
            print "Creating L3 cache block %d..."%(next_l3_cache_id)

            l3cache = sst.Component("l3cache_%d"%(next_l3_cache_id), "memHierarchy.Cache")
            l3cache.addParams(config.getL3Params())
            l3cache.addParams({
                "network_address" : next_network_id,
                "slice_id" : next_l3_cache_id
                })

            connect("l3_%d_link"%next_l3_cache_id,
                    l3cache, "directory",
                    router_map["rtr.%d"%next_network_id], "port2",
                    config.ring_latency)

            next_l3_cache_id = next_l3_cache_id + 1
            next_network_id = next_network_id + 1

        print "Creating Core %d in Group %d"%(next_active_core, next_group)
        if 'miranda' in config.app:
            cpu = sst.Component("cpu%d"%(next_core_id), "miranda.BaseCPU")
            cpu.addParams(config.getCoreConfig(next_core_id))
            cpuPort = "cache_link"
        elif 'ariel' in config.app:
            cpu = arielCPU
            cpuPort = "cache_link_%d"%next_core_id

        l1 = sst.Component("l1cache_%d"%(next_core_id), "memHierarchy.Cache")
        l1.addParams(config.getL1Params())

        l2 = sst.Component("l2cache_%d"%(next_core_id), "memHierarchy.Cache")
        l2.addParams(config.getL2Params())
        l2.addParam( "network_address", next_network_id )

        connect("cpu_cache_link_%d"%next_core_id,
                cpu, cpuPort,
                l1, "high_network_0",
                config.ring_latency).setNoCut()

        connect("l2cache_%d_link"%next_core_id,
                l1, "low_network_0",
                l2, "high_network_0",
                config.ring_latency).setNoCut()

        connect("l2_ring_link_%d"%next_core_id,
                l2, "cache",
                router_map["rtr.%d"%next_network_id], "port2",
                config.ring_latency)

        next_network_id = next_network_id + 1
        next_core_id = next_core_id + 1

    # Connect any remaining L3 cache blocks
    for next_l3_cache_block in range(config.l3_cache_remainder):
        print "Creating L3 cache block: %d..."%(next_l3_cache_id)

        l3cache = sst.Component("l3cache_%d"%(next_l3_cache_id), "memHierarchy.Cache")
        l3cache.addParams(config.getL3Params())

        l3cache.addParams({
            "network_address" : next_network_id,
            "slice_id" : next_l3_cache_id
            })

        connect("l3_%d_link"%next_l3_cache_id,
                l3cache, "directory",
                router_map["rtr.%d"%next_network_id], "port2",
                config.ring_latency)

        next_l3_cache_id = next_l3_cache_id + 1
        next_network_id = next_network_id + 1

    # Connect Memory and Memory Controllers to the ring
    for next_mem_ctrl in range(config.memory_controllers_per_group):	
        mem = sst.Component("memory_%d"%(next_memory_ctrl_id), "memHierarchy.MemController")
        mem.addParams(config.getMemParams())

        dc = sst.Component("dc_%d"%(next_memory_ctrl_id), "memHierarchy.DirectoryController")
        dc.addParams(config.getDCParams(next_memory_ctrl_id))
        dc.addParam("network_address", next_network_id)

        connect("mem_link_%d"%next_memory_ctrl_id,
                mem, "direct_link",
                dc, "memory",
                config.ring_latency)

        connect("dc_link_%d"%next_memory_ctrl_id,
                dc, "network",
                router_map["rtr.%d"%next_network_id], "port2",
                config.ring_latency)

        next_network_id = next_network_id + 1
        next_memory_ctrl_id = next_memory_ctrl_id + 1

    sst.popNamePrefix()

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
