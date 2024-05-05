# Module for reusing params for various components

import os, sys

freq             = "2.0GHz"
cache_line_bytes = "256"
coherence        = "MESI"
replacement      = "lru"

class DCParam:
    def __init__(self, ngroup, mem_per_group=1*1024*1024*1024, interleave_size=4*1024):
        self.ngroup = ngroup
        self.interleave_size = interleave_size
        self.capacity = mem_per_group

    def __getitem__(self, idx):
        if (idx < 0 or idx > self.ngroup-1):
            raise IndexError("Index out of range: {}".format(idx))
        return {
            # Per-Group Params
            "addr_range_start" : idx*self.interleave_size,
            "addr_range_end"   : self.capacity - (self.ngroup-idx-1)*self.interleave_size - 1,
            "interleave_size"  : str(self.interleave_size) + "B",
            "interleave_step"  : str(self.ngroup * self.interleave_size) + "B",
            # Global Params
            #TODO what is the entry cache? Different from mshr?
            "clock"                  : freq,
            "coherence_protocol"     : coherence,
            "entry_cache_size"       : "256*1024*1024",
            "mshr_num_entries"       : 128,
            "access_latency_cycles"  : 2,
            "cache_line_size"        : cache_line_bytes,
        }

class MemCtrlParam:
    def __init__(self, ngroup, mem_per_group=1*1024*1024*1024, interleave_size=4*1024):
        self.ngroup = ngroup
        self.interleave_size = interleave_size
        self.capacity = ngroup * mem_per_group

    def __getitem__(self, idx):
        if (idx < 0 or idx > self.ngroup-1):
            raise IndexError("Index out of range: {}".format(idx))
        return {
            #TODO the Memory Controller is connected to a directory, not the network. Do we
            # need t specify these?
            # Per-Group Params
            "addr_range_start" : idx*self.interleave_size,
            "addr_range_end"   : self.capacity - (self.ngroup-idx-1)*self.interleave_size - 1,
            #"addr_range_end"   : self.capacity - (self.ngroup-idx-1)*self.interleave_size - 1,
            "interleave_size"  : str(self.interleave_size) + "B",
            "interleave_step"  : str(self.ngroup * self.interleave_size) + "B",
            # Global Params
            "clock"   : freq, #TODO: what freq to use?
            "backing" : "none",
        }

class Param:
    def __init__(self, ncpu, ngroup):
        self.dc      = DCParam(ngroup)
        self.memctrl = MemCtrlParam(ngroup)

        self.core = {
                "clock" : "2.4GHz",
                "max_reqs_cycle" : 2,
        }

        self.l1 = {
            "cache_frequency"       : freq,
            "cache_size"            : "4KiB",
            "associativity"         : "4",
            "access_latency_cycles" : "2",
            "L1"                    : "1",
            "cache_line_size"       : cache_line_bytes,
            "coherence_protocol"    : coherence,
            "replacement_policy"    : replacement,
            #"prefetcher"            : "cassini.StridePrefetcher",
            "debug"                 : "0",
        }

        self.l2 = {
            "access_latency_cycles" : "20",
            "cache_frequency"       : freq,
            "replacement_policy"    : replacement,
            "coherence_protocol"    : coherence,
            "associativity"         : "4",
            "cache_line_size"       : cache_line_bytes,
            "prefetcher"            : "cassini.StridePrefetcher",
            "debug"                 : "0",
            "L1"                    : "0",
            "cache_size"            : "1MiB",
            "mshr_latency_cycles"   : "5",
        }

        self.dram = {
            "mem_size" : "1GiB"
        }

        self.bus = {
            "bus_frequency" : freq,
        }
        self.memlink = {
            #"accept_region" : 'false'
            #nothing
        }


        self.mesh_flit           = 36
        self.mesh_link_latency   = "100ps"
        self.freq_mhz            = 2000 #TODO:  use 2200MHz?
        self.mesh_link_bw        = str( (self.freq_mhz*1000*1000*self.mesh_flit) ) + "B/s"
        #self.mesh_link_bw = "2000B/s"
        self.mesh_link_bw = "32GB/s"
        self.network_buffers     = "288B"

        self.noc = {
                "link_bw" : self.mesh_link_bw,
                "flit_size" : str(self.mesh_flit) + "B",
                "input_buf_size" : self.network_buffers,
                "port_priority_equal" : 1,
                "local_ports" : 2,
        }

        self.linkcontrol = {
            "link_bw"      : self.mesh_link_bw,
            "in_buf_size"  : self.network_buffers,
            "out_buf_size" : self.network_buffers,
        }

        self.l2nic = {
            "group":"1",
            #"sources":"1",
            #"destinations":"1",
            #"accept_region" : 0,
        }

        self.dcnic = {
            "group":"2",
            #"sources":"2",
            #"destinations":"2",
            #"accept_region" : 0,
        }
