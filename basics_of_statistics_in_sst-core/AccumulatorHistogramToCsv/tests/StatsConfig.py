# Execute from the command line with the command:
#   sst StatsConfig.py
#
import sst

# Initialize local variables.
#
debug = "0"         # debug level
                    # 0 = FATAL, 1 = WARN, 2 = INFO, 3 = DEBUG, 4 = TRACE, 5 = ALL

componentName0 = "stats_example"

# Set global params for statistics.
#
sst.setStatisticLoadLevel(2)
sst.setStatisticOutput("sst.statOutputCSV",
                       {"filepath":"./StatsOutput.txt",
                        "separator":", "})

# Component definition
#
obj = sst.Component(componentName0, "statsexample.StatsComponent")
obj.addParams({
    "clock"      : "1GHz",
    "debug"      : debug
    })

# Enable the statistics.
#
sst.enableStatisticForComponentName(componentName0,
                                    "INT64_f_clock_ticks", 
                                   {"type":"sst.AccumulatorStatistic",
                                    "rate":"1ns"})
sst.enableStatisticForComponentName(componentName0,
                                    "INT64_r_clock_ticks", 
                                   {"type":"sst.HistogramStatistic",
                                    "rate":"1ns",
                                    "minvalue":"0",
                                    "binwidth":"5",
                                    "numbins":"2"})
