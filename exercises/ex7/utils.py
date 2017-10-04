import sst
import os
import ConfigParser



def connect(name, c0, port0, c1, port1, latency):
    link = sst.Link(name)
    link.connect( (c0, port0, latency), (c1, port1, latency) )
    return link



class SNBConfig:
    def __init__(self, cfgFile, **kwargs):
        cp = ConfigParser.ConfigParser()
        if not cp.read(cfgFile):
            raise Exception('Unable to read file "%s"'%cfgFile)

        self.verbose = "verbose" in kwargs and kwargs["verbose"]

        self.clock = cp.get('CPU', 'clock')
        self.memory_clock = cp.get('Memory', 'clock')
        self.max_reqs_cycle = cp.get('CPU', 'max_reqs_cycle')
        self.coherence_protocol = "MESI"

        self.cores_per_group = cp.getint('Groups', 'cores')
        self.memory_controllers_per_group = cp.getint('Groups', 'memory_controllers')
        self.groups = cp.getint('Groups', 'group_count')
        self.l3cache_blocks_per_group = cp.getint('Groups', 'l3cache_blocks')
        self.l3cache_block_size = cp.get('Groups', 'l3cache_block_size')

        self.total_cores = (self.groups * self.cores_per_group)
        self.num_ring_stop_per_group = self.cores_per_group + self.memory_controllers_per_group + self.l3cache_blocks_per_group
        self.num_ring_stops = self.num_ring_stop_per_group * self.groups

        self.l3_cache_per_core  = int(self.l3cache_blocks_per_group / self.cores_per_group)
        self.l3_cache_remainder = self.l3cache_blocks_per_group - (self.l3_cache_per_core * self.cores_per_group)

        self.ring_latency = cp.get('Network', 'latency')
        self.ring_bandwidth = cp.get('Network', 'bandwidth')
        self.ring_flit_size = cp.get('Network', 'flit_size')

        self.memory_network_bandwidth = cp.get('Memory', 'network_bw')

        self.mem_interleave_size = cp.getint('Memory', 'interleave_size')
        self.memory_capacity = cp.get('Memory', 'capacity')

        self.app = cp.get('CPU', 'application')
        self.coreConfigParams = dict(cp.items(self.app))
        if self.app == 'miranda.STREAMBenchGenerator':
            self.coreConfig = self._streamCoreConfig
        elif 'ariel' in self.app:
            self.coreConfig = self._arielCoreConfig
        else:
            raise Exception("Unknown application '%s'"%app)

    def getCoreConfig(self, core_id):
        params = dict({
                'clock': self.clock,
                'verbose': int(self.verbose)
                })
        params.update(self.coreConfig(core_id))
        return params

    def _arielCoreConfig(self, core_id):
        params = dict({
            "maxcorequeue"        : 256,
            "maxtranscore"        : 16,
            "maxissuepercycle"    : self.max_reqs_cycle,
            "pipetimeout"         : 0,
            "appargcount"         : 0,
            "memorylevels"        : 1,
            "arielinterceptcalls" : 1,
            "arielmode"           : 1,
            "pagecount0"          : 1048576,
            "corecount"           : self.total_cores,
            "defaultlevel"        : 0,
            })
        params.update(self.coreConfigParams)
        if 'executable' not in params:
            if 'OMP_EXE' in os.environ:
                params['OMP_EXE'] = os.environ['OMP_EXE']
            else:
                raise Exception("No Ariel executable specified")
        return params

    def _streamCoreConfig(self, core_id):
        streamN = int(self.coreConfigParams['total_streamn'])
        params = dict()
        params['max_reqs_cycle'] =  self.max_reqs_cycle
        params['generator'] = 'miranda.STREAMBenchGenerator'
        params['generatorParams.n'] = streamN / self.total_cores
        params['generatorParams.start_a'] = ( (streamN * 32) / self.total_cores ) * core_id
        params['generatorParams.start_b'] = ( (streamN * 32) / self.total_cores ) * core_id + (streamN * 32)
        params['generatorParams.start_c'] = ( (streamN * 32) / self.total_cores ) * core_id + (2 * streamN * 32)
        params['generatorParams.operandwidth'] = 32
        params['generatorParams.verbose'] = int(self.verbose)
        return params

    def getL1Params(self):
        return dict({
            "prefetcher": "cassini.StridePrefetcher",
            "prefetcher.reach": 4,
            "prefetcher.detect_range" : 1,
            "cache_frequency": self.clock,
            "cache_size": "32KB",
            "associativity": 8,
            "access_latency_cycles": 4,
            "L1": 1,
            # Default params
            # "cache_line_size": 64,
            # "coherence_protocol": self.coherence_protocol,
            # "replacement_policy": "lru",
            # Not neccessary for simple cases:
            #"maxRequestDelay" : "1000000",
            })

    def getL2Params(self):
        return dict({
            "prefetcher": "cassini.StridePrefetcher",
            "prefetcher.reach": 16,
            "prefetcher.detect_range" : 1,
            "cache_frequency": self.clock,
            "cache_size": "256KB",
            "associativity": 8,
            "access_latency_cycles": 6,
            "mshr_num_entries" : 16,
            "network_bw": self.ring_bandwidth,
            # Default params
            #"cache_line_size": 64,
            #"coherence_protocol": self.coherence_protocol,
            #"replacement_policy": "lru",
            })

    def getL3Params(self):
        return dict({
            "access_latency_cycles" : "12",
            "cache_frequency" : self.clock,
            "associativity" : "16",
            "cache_size" : self.l3cache_block_size,
            "mshr_num_entries" : "4096",
            "network_bw": self.ring_bandwidth,
            # Distributed caches
            "num_cache_slices" : self.groups * self.l3cache_blocks_per_group,
            "slice_allocation_policy" : "rr",
            # Default params
            # "replacement_policy" : "lru",
            # "cache_line_size" : "64",
            # "coherence_protocol" : coherence_protocol,
            })

    def getMemParams(self):
        return dict({
            "backend" : "memHierarchy.simpleMem",
            "backend.access_time" : "30ns",
            "backend.mem_size" : "%d%s"%(int(filter(str.isdigit, self.memory_capacity)) / (self.groups * self.memory_controllers_per_group), filter(str.isalpha, self.memory_capacity)),
            "clock" : self.memory_clock,
            "network_bw": self.ring_bandwidth,
            "do_not_back" : 1,
            })

    def getDCParams(self, dc_id):
        return dict({
            "interleave_size": "%dB"%self.mem_interleave_size,
            "interleave_step": "%dB"%((self.groups * self.memory_controllers_per_group) * (self.mem_interleave_size)),
            "entry_cache_size": 256*1024*1024, #Entry cache size of mem/blocksize
            "clock": self.memory_clock,
            "network_bw": self.ring_bandwidth,
            "addr_range_start" : dc_id * self.mem_interleave_size,
            "addr_range_end" : (int(filter(str.isdigit, self.memory_capacity)) * 1024 * 1024) - (self.groups * self.memory_controllers_per_group * self.mem_interleave_size) + (dc_id * self.mem_interleave_size)
            # Default params
            # "coherence_protocol": coherence_protocol,
            })

    def getRingParams(self):
        return dict({
            "torus:shape" : self.groups * (self.cores_per_group + self.memory_controllers_per_group + self.l3cache_blocks_per_group),
            "output_latency" : "25ps",
            "xbar_bw" : self.ring_bandwidth,
            "input_buf_size" : "2KB",
            "input_latency" : "25ps",
            "num_ports" : "3",
            "torus:local_ports" : "1",
            "flit_size" : self.ring_flit_size,
            "output_buf_size" : "2KB",
            "link_bw" : self.ring_bandwidth,
            "torus:width" : "1",
            "topology" : "merlin.torus"
        })
