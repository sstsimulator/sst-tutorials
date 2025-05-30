import sst

numCores = 2
numLLC = 2

memory_mb = 1024
memory_capacity_inB = memory_mb * 1024 * 1024

memory_per_block = memory_mb / numLLC
memory_capacity_block_inB = memory_per_block * 1024 * 1024

#########################################################################
## Define SST core options
#########################################################################
# If this demo gets to 100ms, something has gone very wrong!
sst.setProgramOption("stop-at", "100ms")


#########################################################################
## Parameter Definitions
#########################################################################
# Cache: L1, 2.4GHz, 2KB, 4-way set associative, 64B lines, LRU replacement, MESI coherence
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

l2_params = dict({
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

# Core: 2.4GHz, 2 accesses/cycle, STREAM (triad) pattern generator with 1000 elements per array
core_params = dict({
    "clock" : "2.4GHz",
    "max_reqs_cycle" : 2,
})

gen_params = dict({
    "n" : 1000,             # Number of array elements
})


#########################################################################
## Getting too many components to write out everything, so let's
## simplify the description of the model...
#########################################################################
bus = sst.Component("bus", "memHierarchy.Bus")
bus.addParams({ "bus_frequency" : "1.0Ghz" })


for core_id in range (0, numCores):
   core = sst.Component("core_" + str(core_id), "miranda.BaseCPU")
   core.addParams(core_params)

   gen = core.setSubComponent("generator", "miranda.STREAMBenchGenerator")
   gen.addParams(gen_params)

   l1_cache = sst.Component("l1_cache_" + str(core_id), "memHierarchy.Cache")
   l1_cache.addParams(l1_params)

   l2_cache = sst.Component("l2_cache_" + str(core_id), "memHierarchy.Cache")
   l2_cache.addParams(l2_params)

   ## Define and connect links
   core_cache_link = sst.Link("core_to_cache_" + str(core_id))
   core_cache_link.connect( (core, "cache_link", "100ps"), (l1_cache, "highlink", "100ps") )

   l1_l2_link = sst.Link("l1_to_l2_" + str(core_id))
   l1_l2_link.connect( (l1_cache, "lowlink", "100ps"), (l2_cache, "highlink", "100ps") )

   bus_l2 = sst.Link("bus_to_l2_" + str(core_id))
   bus_l2.connect( (l2_cache, "lowlink", "100ps"), (bus, "highlink" + str(core_id), "100ps") )


memctrl = sst.Component("memory", "memHierarchy.MemController")
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


link_dir_mem = sst.Link("link_dir_mem_0")
link_dir_mem.connect( (bus, "lowlink0", "50ps"), (memctrl, "highlink", "50ps") )


#########################################################################
## Statistics
#########################################################################

# Enable SST Statistics Outputs for this simulation
# Generate statistics in CSV format
sst.setStatisticOutput("sst.statoutputcsv")

# Send the statistics to a fiel called 'stats.csv'
sst.setStatisticOutputOptions( { "filepath"  : "stats_demo-6.csv" })

# Print statistics of level 5 and below (0-5)
sst.setStatisticLoadLevel(5)

# Enable statistics for all the component
sst.enableAllStatisticsForAllComponents()

print ("\nCompleted configuring the Demo_6 model\n")

################################ The End ################################



