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
sst.setStatisticLoadLevel(7)
sst.setStatisticOutput("sst.statOutputConsole")

# Component definition
#
obj = sst.Component(componentName0, "statsexample.StatsComponent")
obj.addParams({
    "clock"      : "1GHz",
    "debug"      : debug
    })

# Enable the statistics.
#
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic",
                                         "rate":"1ns"}) 
