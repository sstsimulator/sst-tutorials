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
        self.max_reqs_cycle = cp.get('CPU', 'max_reqs_cycle')

        self.memory_clock = cp.get('Memory', 'clock')
        self.memory_network_bandwidth = cp.get('Memory', 'network_bw')
        self.memory_capacity = cp.get('Memory', 'capacity')
        self.cache_link_latency = cp.get('Memory', 'link_latency')
        self.coherence_protocol = "MESI"

        self.app = cp.get('CPU', 'application')
        self.coreConfigParams = dict(cp.items(self.app))
        if self.app == 'miranda.STREAMBenchGenerator':
            self.coreConfig = self._streamCoreConfig
        else:
            raise Exception("Unknown application '%s'"%app)

    def getCoreConfig(self, core_id):
        params = dict({
                'clock': self.clock,
                'verbose': int(self.verbose)
                })
        params.update(self.coreConfig(core_id))
        return params

    def _streamCoreConfig(self, core_id):
        streamN = int(self.coreConfigParams['total_streamn'])
        params = dict()
        params['max_reqs_cycle'] =  self.max_reqs_cycle
        params['generator'] = 'miranda.STREAMBenchGenerator'
        params['generatorParams.n'] = streamN
        params['generatorParams.start_a'] = 0 
        params['generatorParams.start_b'] = streamN * 32
        params['generatorParams.start_c'] = 2 * streamN * 32
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
            "do_not_back" : 1,
            })
