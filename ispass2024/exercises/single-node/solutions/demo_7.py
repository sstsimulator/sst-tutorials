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
sst.setProgramOption("stopAtCycle", "100ms")


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
shogun_xbar = sst.Component("shogunxbar", "shogun.ShogunXBar")
shogun_xbar.addParams({
       "clock" : "1.0GHz",
       "port_count" : 4,
       "verbose" : 0
})

p_n = 0

for core_id in range (0, numCores):
   core = sst.Component("core_" + str(core_id), "miranda.BaseCPU")
   core.addParams(core_params)

   gen = core.setSubComponent("generator", "miranda.STREAMBenchGenerator")
   gen.addParams(gen_params)

   iface = core.setSubComponent("memory", "memHierarchy.memInterface")

   l1_cache = sst.Component("l1_cache_" + str(core_id), "memHierarchy.Cache")
   l1_cache.addParams(l1_params)
   l1_up = l1_cache.setSubComponent("cpulink", "memHierarchy.MemLink")
   l1_down = l1_cache.setSubComponent("memlink", "memHierarchy.MemLink")

   l2_cache = sst.Component("l2_cache_" + str(core_id), "memHierarchy.Cache")
   l2_cache.addParams(l2_params)

   l2_up = l2_cache.setSubComponent("cpulink", "memHierarchy.MemLink")
   l2_down = l2_cache.setSubComponent("memlink", "memHierarchy.MemNIC")
   l2_down.addParams({ "group" : 2 })
   l2_linkctrl = l2_down.setSubComponent("linkcontrol", "shogun.ShogunNIC")

   ## Define and connect links
   core_cache_link = sst.Link("core_to_cache_" + str(core_id))
   core_cache_link.connect( (iface, "port", "100ps"), (l1_up, "port", "100ps") )

   l1_l2_link = sst.Link("l1_to_l2_" + str(core_id))
   l1_l2_link.connect( (l1_down, "port", "100ps"), (l2_up, "port", "100ps") )

   bus_l2 = sst.Link("bus_to_l2_" + str(core_id))
   bus_l2.connect( (l2_linkctrl, "port", "100ps"), (shogun_xbar, "port" + str(p_n), "100ps") )

   p_n = p_n + 1

for cache_id in range (0, numLLC):
   startAddr = 0 + (256 * cache_id)
   endAddr = startAddr + memory_capacity_inB - (256 * numLLC)

   dirctrl = sst.Component("dirctrl_" + str(cache_id), "memHierarchy.DirectoryController")
   dirctrl.addParams({
         "coherence_protocol" : "MESI",
         "entry_cache_size" : "32768",
         "addr_range_end" : endAddr,
         "addr_range_start" : startAddr,
         "interleave_size" : "256B",
         "interleave_step" : str(numLLC * 256) + "B",
   })
   dc_cpulink = dirctrl.setSubComponent("cpulink", "memHierarchy.MemNIC")
   dc_memlink = dirctrl.setSubComponent("memlink", "memHierarchy.MemLink")
   dc_cpulink.addParams({
      "group" : 3,
   })
   dc_linkctrl = dc_cpulink.setSubComponent("linkcontrol", "shogun.ShogunNIC")

   memctrl = sst.Component("memory_" + str(cache_id), "memHierarchy.MemController")
   memctrl.addParams({
      "clock" : "1.0GHz",
      "backing" : "none", # We're not using real memory values, just addresses
   })
   memLink = memctrl.setSubComponent("cpulink", "memHierarchy.MemLink")

   memory = memctrl.setSubComponent("backend", "memHierarchy.timingDRAM")
   memory.addParams({
      "id" : 0,
      "mem_size" : str(memory_capacity_inB) + "B",
      "clock" : "1.0GHz",
   })

   bus_mem = sst.Link("cache_to_memory_" + str(cache_id))
   bus_mem.connect( (shogun_xbar, "port" + str(p_n), "100ps"), (dc_linkctrl, "port", "100ps") )

   link_dir_mem = sst.Link("link_dir_mem_" + str(cache_id))
   link_dir_mem.connect( (dc_memlink, "port", "50ps"), (memLink, "port", "50ps") )

   p_n = p_n + 1



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

print ("\nCompleted configuring the Demo_7 model\n")


################################ The End ################################

