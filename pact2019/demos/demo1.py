import sst
import sys


verbose = False

# Build Configuration Information
clock = "2.4GHz"
max_reqs_cycle =  2
streamN = 1000

cache_link_latency = "300ps"


# Connect Cores & caches
print "Configuring core..."

# Create core using Miranda model
core = sst.Component("core", "miranda.BaseCPU")
core.addParams({
    'clock':                clock,
    'verbose':              int(verbose),
    'max_reqs_cycle':       max_reqs_cycle,
})

# Set and configure a memory access pattern generator on the core
gen = core.setSubComponent("generator", "miranda.STREAMBenchGenerator")
gen.addParams({
    'n':            streamN,
    'start_a':      0,                  # Start address for array A
    'start_b':      streamN * 32,       # Start address for array B
    'start_c':      2 * streamN * 32,   # Start address for array C
    'operandwidth': 32,                 # Size of array elements
    'verbose':      int(verbose),
}) 

l1 = sst.Component("L1", "memHierarchy.Cache")
l1.addParams({
    'cache_frequency':          clock,
    'cache_size':               "2KB",
    'associativity':            4,
    'access_latency_cycles':    2,
    'L1':                       1,
    # Default params
    # "cache_line_size": 64,
    # "coherence_protocol": "MESI",
    # "replacement_policy": "lru",
    # Not neccessary for simple cases:
    #"maxRequestDelay" : "1000000",
})

print "Configuring Memory"
mctrl = sst.Component("memctrl", "memHierarchy.MemController")
mctrl.addParams({
    "clock" :   "1GHz",
    "backing" : "none",  # Don't allocate space for data values; Miranda doesn't use them
})

# Use the 'SimpleMem' constant latency memory timing model in our memory
memory = mctrl.setSubComponent("backend", "memHierarchy.simpleMem")
memory.addParams({
    "access_time" : "50ns",
    "mem_size" :    "1GiB",
})

# link0: core <-> cache
link0 = sst.Link("core_to_cache")
link0.connect((core, "cache_link", cache_link_latency), (l1, "high_network_0", cache_link_latency))

# link1: cache <-> memory
link1 = sst.Link("cache_to_memory")
link1.connect(( l1, "low_network_0", cache_link_latency), (mctrl, "direct_link", cache_link_latency))


# ===============================================================================

# Enable SST Statistics Outputs for this simulation
# Generate statistics in CSV format
sst.setStatisticOutput("sst.statoutputcsv")

# Send the statistics to a fiel called 'stats.csv'
sst.setStatisticOutputOptions( { "filepath"  : "stats.csv" })

# Print statistics of level 5 and below (0-5)
sst.setStatisticLoadLevel(5)

# Enable statistics for all the component
sst.enableAllStatisticsForAllComponents()

print "Completed configuring the Demo1 model"


