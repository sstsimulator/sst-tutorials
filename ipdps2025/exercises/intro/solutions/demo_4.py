import sst

#########################################################################
## Define SST core options
#########################################################################
# If this demo gets to 100ms, something has gone very wrong!
sst.setProgramOption("stop-at", "100ms")


#########################################################################
## Declare components
#########################################################################
core = sst.Component("core", "miranda.BaseCPU")
l1_cache = sst.Component("l1_cache", "memHierarchy.Cache")
memctrl = sst.Component("memory", "memHierarchy.MemController")

l2_cache = sst.Component("l2_cache", "memHierarchy.Cache")

#########################################################################
## Set component parameters and fill subcomponent slots
#########################################################################
# Core: 2.4GHz, 2 accesses/cycle, STREAM (triad) pattern generator with 1000 elements per array
core.addParams({
    "clock" : "2.4GHz",
    "max_reqs_cycle" : 2,
})
gen = core.setSubComponent("generator", "miranda.STREAMBenchGenerator")
gen.addParams({
    "n" : 1000,             # Number of array elements
})

# Cache: L1, 2.4GHz, 2KB, 4-way set associative, 64B lines, LRU replacement, MESI coherence
l1_cache.addParams({
    "L1" : 1,
    "cache_frequency" : "2.4GHz",
    "access_latency_cycles" : 2,
    "cache_size" : "2KiB",
    "associativity" : 4,
    "replacement_policy" : "lru",
    "coherence_policy" : "MESI",
    "cache_line_size" : 64,
})

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
    "addr_range_end" : 1024*1024*1024-1,
})

memory = memctrl.setSubComponent("backend", "memHierarchy.timingDRAM")
memory.addParams({
    "id" : 0,
    "mem_size" : "1GiB",
    "clock" : "1.0GHz",
})


#########################################################################
## Declare links
#########################################################################
core_cache = sst.Link("core_to_cache")
l1_l2_cache = sst.Link("l1_to_l2")
llc_mem = sst.Link("cache_to_memory")

#########################################################################
## Connect components with the links
#########################################################################
core_cache.connect( (core, "cache_link", "100ps"), (l1_cache, "highlink", "100ps") )
l1_l2_cache.connect( (l1_cache, "lowlink", "100ps"), (l2_cache, "highlink", "100ps") )
llc_mem.connect( (l2_cache, "lowlink", "100ps"), (memctrl, "highlink", "100ps") )


#########################################################################
## Statistics
#########################################################################

# Enable SST Statistics Outputs for this simulation
# Generate statistics in CSV format
sst.setStatisticOutput("sst.statoutputcsv")

# Send the statistics to a fiel called 'stats.csv'
sst.setStatisticOutputOptions( { "filepath"  : "stats_demo-4.csv" })

# Print statistics of level 5 and below (0-5)
sst.setStatisticLoadLevel(5)

# Enable statistics for all the component
sst.enableAllStatisticsForAllComponents()

print ("\nCompleted configuring the Demo_4 model\n")

################################ The End ################################



