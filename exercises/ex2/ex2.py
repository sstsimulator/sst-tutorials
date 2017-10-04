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
config = Config(cfgFile, verbose=verbose)

# Connect Cores & caches
print "Configuring CPU..."

cpu = sst.Component("cpu", "miranda.BaseCPU")
cpu.addParams(config.getCoreConfig(0))

l1 = sst.Component("l1cache", "memHierarchy.Cache")
l1.addParams(config.getL1Params())

l2 = sst.Component("l2cache", "memHierarchy.Cache")
l2.addParams(config.getL2Params())

connect("cpu_cache_link",
        cpu, "cache_link",
        l1, "high_network_0",
        config.cache_link_latency).setNoCut()

connect("l2cache_link",
        l1, "low_network_0",
        l2, "high_network_0",
        config.cache_link_latency).setNoCut()


# Connect Memory and Memory Controller to the ring
print "Configuring Memory"
mem = sst.Component("memory", "memHierarchy.MemController")
mem.addParams(config.getMemParams())

connect("l2cache_mem_link",
        l2, "low_network_0",
        mem, "direct_link",
        config.cache_link_latency)


# ===============================================================================

# Enable SST Statistics Outputs for this simulation
sst.setStatisticLoadLevel(statLevel)
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic"})

sst.setStatisticOutput("sst.statOutputCSV")
sst.setStatisticOutputOptions( {
    "filepath"  : statFile,
    "separator" : ", "
    } )

print "Completed configuring the EX2 model"
