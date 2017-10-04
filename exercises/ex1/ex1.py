import sst
import sys


verbose = False
statFile = "stats.csv"
statLevel = 16

# Build Configuration Information
clock = "2660MHz"
max_reqs_cycle =  3
streamN = 500000

memory_clock = "200MHz"
memory_capacity = "16384MiB"

cache_link_latency = "300ps"


# Connect Cores & caches
print "Configuring CPU..."

cpu = sst.Component("cpu", "miranda.BaseCPU")
cpu.addParams({
        'clock':                        clock,
        'verbose':                      int(verbose),
        'max_reqs_cycle':               max_reqs_cycle,
        'generator':                    'miranda.STREAMBenchGenerator',
        'generatorParams.n':            streamN,
        'generatorParams.start_a':      0,
        'generatorParams.start_b':      streamN * 32,
        'generatorParams.start_c':      2 * streamN * 32,
        'generatorParams.operandwidth': 32,
        'generatorParams.verbose':      int(verbose),
})

l1 = sst.Component("l1cache", "memHierarchy.Cache")
l1.addParams({
        "prefetcher": "cassini.StridePrefetcher",
        "prefetcher.reach":         4,
        "prefetcher.detect_range":  1,
        "cache_frequency":          clock,
        "cache_size":               "32KB",
        "associativity":            8,
        "access_latency_cycles":    4,
        "L1":                       1,
        # Default params
        # "cache_line_size": 64,
        # "coherence_protocol": "MESI",
        # "replacement_policy": "lru",
        # Not neccessary for simple cases:
        #"maxRequestDelay" : "1000000",
        })

l2 = sst.Component("l2cache", "memHierarchy.Cache")
l2.addParams({
        "prefetcher": "cassini.StridePrefetcher",
        "prefetcher.reach":         16,
        "prefetcher.detect_range" : 1,
        "cache_frequency":          clock,
        "cache_size":               "256KB",
        "associativity":            8,
        "access_latency_cycles":    6,
        "mshr_num_entries" :        16,
        # Default params
        #"cache_line_size": 64,
        #"coherence_protocol": "MESI",
        #"replacement_policy": "lru",
        })


print "Configuring Memory"
mem = sst.Component("memory", "memHierarchy.MemController")
mem.addParams({
        "backend" : "memHierarchy.simpleMem",
        "backend.access_time" : "30ns",
        "backend.mem_size" : memory_capacity,
        "clock" : memory_clock,
        "do_not_back" : 1,
        })



cpu_cache_link = sst.Link("cpu_cache_link")
cpu_cache_link.connect((cpu, "cache_link", cache_link_latency), (l1, "high_network_0", cache_link_latency))

l2_cache_link = sst.Link("l2cache_link")
l2_cache_link.connect((l1, "low_network_0", cache_link_latency), (l2, "high_network_0", cache_link_latency))

mem_link = sst.Link("l2cache_mem_link")
mem_link.connect(( l2, "low_network_0", cache_link_latency), (mem, "direct_link", cache_link_latency))


# ===============================================================================

# Enable SST Statistics Outputs for this simulation
sst.setStatisticLoadLevel(statLevel)
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic"})

sst.setStatisticOutput("sst.statOutputCSV")
sst.setStatisticOutputOptions( {
    "filepath"  : statFile,
    "separator" : ", "
    } )

print "Completed configuring the EX1 model"


