import sst

#########################################################################
## Define SST core options
#########################################################################
# If this demo gets to 100ms, something has gone very wrong!
sst.setProgramOption("stopAtCycle", "100ms")


#########################################################################
## Declare components
#########################################################################
core_0 = sst.Component("core_0", "miranda.BaseCPU")
core_1 = sst.Component("core_1", "miranda.BaseCPU")

l1_cache_0 = sst.Component("l1_cache_0", "memHierarchy.Cache")
l1_cache_1 = sst.Component("l1_cache_1", "memHierarchy.Cache")
l2_cache = sst.Component("l2_cache", "memHierarchy.Cache")

memctrl = sst.Component("memory", "memHierarchy.MemController")

bus = sst.Component("bus", "memHierarchy.Bus")

#########################################################################
## Parameter Definitions
#########################################################################
l1_params = dict({
    "L1" : 1,
    "cache_frequency" : "2.4GHz",
    "access_latency_cycles" : 2,
    "cache_size" : "2KiB",
    "associativity" : 4,
    "replacement_policy" : "lru",
    "coherence_policy" : "MESI",
    "cache_line_size" : 64,
})


#########################################################################
## Set component parameters and fill subcomponent slots
#########################################################################
# Core: 2.4GHz, 2 accesses/cycle, STREAM (triad) pattern generator with 1000 elements per array
core_0.addParams({
    "clock" : "2.4GHz",
    "max_reqs_cycle" : 2,
})

core_1.addParams({
    "clock" : "2.4GHz",
    "max_reqs_cycle" : 2,
})

gen_0 = core_0.setSubComponent("generator", "miranda.STREAMBenchGenerator")
gen_0.addParams({
    "n" : 1000,             # Number of array elements
})

gen_1 = core_1.setSubComponent("generator", "miranda.STREAMBenchGenerator")
gen_1.addParams({
    "n" : 1000,             # Number of array elements
})

# Cache: L1, 2.4GHz, 2KB, 4-way set associative, 64B lines, LRU replacement, MESI coherence
l1_cache_0.addParams(l1_params)
l1_cache_1.addParams(l1_params)

l2_cache.addParams({
    "L1" : 0,
    "cache_frequency" : "2.4GHz",
    "access_latency_cycles" : 10,
    "cache_size" : "64KiB",
    "associativity" : 4,
    "replacement_policy" : "lru",
    "coherence_policy" : "MESI",
    "cache_line_size" : 64,
    "mshr_latency_cycles" : 4,
    "mshr_num_entries"  : 256
})

# Memory: 50ns access, 1GB
memctrl.addParams({
    "clock" : "1.0GHz",
    "backing" : "none", # We're not using real memory values, just addresses
})

memory = memctrl.setSubComponent("backend", "memHierarchy.timingDRAM")
memory.addParams({
    "id" : 0,
    "mem_size" : "1GiB",
    "clock" : "1.0GHz",
})

bus.addParams({ "bus_frequency" : "1.0Ghz" })

#########################################################################
## Declare links
#########################################################################
core_0_cache_0 = sst.Link("core_to_cache_0")
core_1_cache_1 = sst.Link("core_to_cache_1")
l1_0_bus = sst.Link("l1_to_bus_0")
l1_1_bus = sst.Link("l1_to_bus_1")
bus_l2 = sst.Link("bus_to_l2")
llc_mem = sst.Link("cache_to_memory")

#########################################################################
## Connect components with the links
#########################################################################
core_0_cache_0.connect( (core_0, "cache_link", "100ps"), (l1_cache_0, "high_network_0", "100ps") )
core_1_cache_1.connect( (core_1, "cache_link", "100ps"), (l1_cache_1, "high_network_0", "100ps") )

l1_0_bus.connect( (l1_cache_0, "low_network_0", "100ps"), (bus, "high_network_0", "100ps") )
l1_1_bus.connect( (l1_cache_1, "low_network_0", "100ps"), (bus, "high_network_1", "100ps") )

bus_l2.connect( (bus, "low_network_0", "100ps"), (l2_cache, "high_network_0", "100ps") )
llc_mem.connect( (l2_cache, "low_network_0", "100ps"), (memctrl, "direct_link", "100ps") )

#########################################################################
## Statistics
#########################################################################

# Enable SST Statistics Outputs for this simulation
# Generate statistics in CSV format
sst.setStatisticOutput("sst.statoutputcsv")

# Send the statistics to a fiel called 'stats.csv'
sst.setStatisticOutputOptions( { "filepath"  : "stats_demo-5.csv" })

# Print statistics of level 5 and below (0-5)
sst.setStatisticLoadLevel(5)

# Enable statistics for all the component
sst.enableAllStatisticsForAllComponents()

print "\nCompleted configuring the Demo_5 model\n"

################################ The End ################################



