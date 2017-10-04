import sst
import os
import ConfigParser



def connect(name, c0, port0, c1, port1, latency):
    link = sst.Link(name)
    link.connect( (c0, port0, latency), (c1, port1, latency) )
    return link



class Config:
    def __init__(self, cfgFile, **kwargs):
        cp = ConfigParser.ConfigParser()
        if not cp.read(cfgFile):
            raise Exception('Unable to read file "%s"'%cfgFile)

        self.verbose = "verbose" in kwargs and kwargs["verbose"]

        self.clock = cp.get('CPU', 'clock')
        self.total_cores = cp.getint('CPU', 'num_cores')
        self.max_reqs_cycle = cp.get('CPU', 'max_reqs_cycle')

        self.memory_clock = cp.get('Memory', 'clock')
        self.memory_network_bandwidth = cp.get('Memory', 'network_bw')
        self.memory_capacity = cp.get('Memory', 'capacity')
        self.coherence_protocol = "MESI"

        self.num_ring_stops = self.total_cores + 1

        self.ring_latency = cp.get('Network', 'latency')
        self.ring_bandwidth = cp.get('Network', 'bandwidth')
        self.ring_flit_size = cp.get('Network', 'flit_size')


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

    def getMemParams(self):
        return dict({
            "backend" : "memHierarchy.simpleMem",
            "backend.access_time" : "30ns",
            "backend.mem_size" : self.memory_capacity,
            "clock" : self.memory_clock,
            "network_bw": self.ring_bandwidth,
            "do_not_back" : 1,
            })

    def getDCParams(self, dc_id):
        return dict({
            "entry_cache_size": 256*1024*1024, #Entry cache size of mem/blocksize
            "clock": self.memory_clock,
            "network_bw": self.ring_bandwidth,
            "addr_range_start" : 0,
            "addr_range_end" : (int(filter(str.isdigit, self.memory_capacity)) * 1024 * 1024),
            # Default params
            # "coherence_protocol": coherence_protocol,
            })

    def getRingParams(self):
        return dict({
            "torus:shape" : self.num_ring_stops,
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
