# Using Statistics With The Structured Simulation Toolkit (SST)

## Introduction

Statistics are a significant part of evaluating simulation results.  For this reason, SST has included an interface for generating component statistics since SST V5.0.  Its purpose is to provide a consistent way to generate and display simulation results, rather than having each developer create their own.

This document is intended as an introduction to statistics within SST.  It will describe how to integrate statistics as part of an SST component, how to enable them from within the Python configuration file, and how to control their display.

## Adding Statistics to Components and SubComponents

Statistics are associated with components and subcomponents.  They are defined as part of the component/subcomponent and enabled/disable as necessary in the Python configuration file.

There are four standard types of statistics included as part of SST.

Accumulator - An Accumulator is a simple statistic that stores a simulation value's cumulative sum (x1 + x2 + ...), the sum squared (x1^2 + x2^2 + ...), and data count.  It has the following parameters.

* type - A constant set to sst.AccumulatorStatistic.
* rate - The rate at which statistics are printed.  This parameter must include the units.  So, for example, to print statistics every nanosecond the rate is specified as "1ns" or "1000ps".  If the rate parameter is not provided or set to 0, statistics are printed at the end of the simulation.
* startat - The simulation time at which the statistic is enabled.  This parameter must include the units.  So, for example, to enable statistics at a simulation time of 5 nanosec, the startat parameter is specified as "5ns" or "5000ps".  If the startat parameter is not provided or set to 0, the statistic is enabled as soon as the simulation starts.
* stopat - The simulation time at which the statistic is disabled.  This parameter must include the units.  So, for example, to disable statistics at a simulation time of 5 nanosec, the stopat parameter is specified as "5ns" or "5000ps".  If the stopat parameter is not provided or set to 0, the statistic will be enabled for the length of the simulation.

Histogram - A Histogram is a representation of data distribution over a numerical range.  Given the number of intervals and their width, values are assigned to the appropriate interval ("binned"), allowing for the calculation of frequency or probability distributions.  It's assumed that the lowest bin's range will begin at zero and a bin's range does not include its upper limit (i.e., a bin with range 0-5 will include all values >= 0 and < 5).

The histogram statistic has the following parameters:

* type - A constant set to sst.AccumulatorStatistic.
* rate - The rate at which statistics are printed.  This parameter must include the units.  So, for example, to print statistics every nanosecond the rate is specified as "1ns" or "1000ps".  If the rate parameter is not provided or set to 0, statistics are printed at the end of the simulation.
* startat - The simulation time at which the statistic is enabled.  This parameter must include the units.  So, for example, to enable statistics at a simulation time of 5 nanosec, the startat parameter is specified as "5ns" or "5000ps".  If the startat parameter is not provided or set to 0, the statistic is enabled as soon as the simulation starts.
* stopat - The simulation time at which the statistic is disabled.  This parameter must include the units.  So, for example, to disable statistics at a simulation time of 5 nanosec, the stopat parameter is specified as "5ns" or "5000ps".  If the stopat parameter is not provided or set to 0, the statistic will be enabled for the length of the simulation.
* minvalue - The lowest statistic value to be binned.  This value must be set to 0.
* binwidth - Numeric width of each bin.
* numbins - The number of bins.  
* dumpbinsonoutput - A flag indicating the bin counts should be include each time the statistics are printed.  Default is "on".  Set to "off" to turn this flag off.
* includeoutofbounds - A flag indicating if data that falls outside the bin values should be counted.  When enabled, two additional bins for tracking low out-of-bounds values ("NumOutOfBounds-MinValue") and high out-of-band-values ("NumOutOfBounds-MaxValue") are included.  Default is "on".  Set to "off" to turn this flag off.

Unique Count - A Unique Count statistic counts the number of unique values associated with a statistic and has the following parameters:

* type - A constant set to sst.UniqueCountStatistic.
* rate - The rate at which statistics are printed.  This parameter must include the units.  So, for example, to print statistics every nanosecond the rate is specified as "1ns" or "1000ps".  If the rate parameter is not provided or set to 0, statistics are printed at the end of the simulation.
* startat - The simulation time at which the statistic is enabled.  This parameter must include the units.  So, for example, to enable statistics at a simulation time of 5 nanosec, the startat parameter is specified as "5ns" or "5000ps".  If the startat parameter is not provided or set to 0, the statistic is enabled as soon as the simulation starts.
* stopat - The simulation time at which the statistic is disabled.  This parameter must include the units.  So, for example, to disable statistics at a simulation time of 5 nanosec, the stopat parameter is specified as "5ns" or "5000ps".  If the stopat parameter is not provided or set to 0, the statistic will be enabled for the length of the simulation.

Null - The Null statistic is simply a place holder that does nothing.  Its purpose is to serve as a return value should the attempt to register one of the other statistics fail.  In that case, any of the method calls associated with a statistic can still be made without having the simulation fail.  As such, it has no associated parameters.

A statistic is defined within a component/subcomponent using the SST_ELI_DOCUMENT_STATISTICS macro.

```
    SST_ELI_DOCUMENT_STATISTICS( 
        {"Clock_Ticks", "Statistic that records the number of clock tics", "ticks", 2},
    )
```
Fields included are the statistic name, description, units, and the level at which the statistic will be enabled.  The statistic name is used to reference the statistic from the simulation's Python configuration file.  The description is a friendly explanation of the statistics purpose.  Units are a string associated with the value for which statistics are being gathered.  The enable level is similar to the log level used for error logging.  Using the enable level, one can enable or disable statistics based on a desired simulation load level, increasing or decreasing the verbosity of reported statistics.

Once the statistic is defined it's necessary to create a statistic object and register it with the simulation using the registerStatistic() method.

```
    Statistic<uint64_t>* statistic;
    statistic = registerStatistic<uint64_t>("Clock_Ticks");
```
The type associated with the statistic object specifies the data type to be tracked by the statistic and can be any numeric data type.  To add data to a statistic, use the addData() method associated with the statistic object.

```
    statistic->addData(datum);
```
## Configuring a Statistic

As mentioned previously, a statistic is defined as part of a component or subcomponent.  However, it is configured and enabled as part of the simulation Python configuration file.

Configuring statistics consists of three steps, setting the simulation's statistic load level, setting the statistic output, and enabling the statistic.  Each of these is discussed below.

### Setting the Statistic Load Level

One of the fields included when defining a statistic using the `SST_ELI_DOCUMENT_STATISTICS` macro is the statistic load level.  This value is comparable to a system log level and is used to control the statistic verbosity.

A simulation's statistic load level is set in the Python configuration file using the sst.setStatisticLoadLevel() method.  
```
sst.setStatisticLoadLevel(<statistic_load_level>)
```
Any statistic whose defined load level is less than or equal to the load level set in the Python configuration file will be generated.

### Setting the Statistics Output

The output location for generated statistics is set using the sst.setStatisticOutput() method.
```
sst.setStatisticOutput(<statistic_output_type>)
```
There are five standard ways within SST to save or display statistics.  They can be sent to the console, to a file as text or comma separated values, or to a compressed file as text or comma separated values.  Each of these cases is described below.

#### Sending Statistics to the Console

Sending statistics to the console is the easiest option.  Simply specify the statistic output as sst.statOutputConsole.
```
sst.setStatisticOutput(sst.statOutputConsole)
```
No additional parameters are required.

A sample accumulator statistic whose output is written to the console is shown below.
```
stats_example.INT64_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9; 
```

#### Sending Statistics to a Text File

To send statistics to a text file, specify the statistic output type as sst.statOutputTXT.  The associated parameters and their meaning are defined below.

* filepath - Path to the text file where the statistics are written.  The default file path is "./StatisticOutput.txt"
* outputtopheader - If set to "1", a top-level header is written to the output file.  This header contains the name of each registered statistics field.  The default is to set the flag to "0" (no top header).
* outputinlineheader - If set to "1", the field names are printed inline with the statistics data.  The default is to set the flag to "1" (display the inline header).
* outputsimtime - If set to "1", the simulation time is included as one of the statistics fields.  The default is to set the flag to "1" (display the simulation time).
* outputrank - if set to "1", the MPI rank is included as one of the statistics fields.  The default is to set the flag to "1" (display the MPI rank).

Below is an SST output specification that sends the statistics to a file named TxtOutput.txt.  The file will include the simulation time, MPI rank, and inline field names.  

```
 sst.setStatisticOutput("sst.statOutputTXT", {"filepath": "./TxtOutput.txt",
                                              "outputtopheader": "0",
                                              "outputinlineheader": "1",
                                              "outputrank": "1",
                                              "outputsimtime": "1"})
```
The contents of the file are shown below.

```
stats_example.INT64_clock_ticks : Accumulator : SimTime = 10000; Rank = 0; Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9;
```

#### Sending Statistics to a CSV File

To send statistics to a csv file, specify the statistic output type as sst.statOutputCSV.  The associated parameters and their meaning are defined below.

* filepath - Path to the text file where the statistics are written.  The default file path is "./StatisticOutput.txt".
* separator - Character used to separate values on a single line in the file.  The default separator is ",".
* outputtopheader - If set to "1", a top-level header is written to the output file.  This header contains the name of each registered field for all statistics.  The default is to set the flag to "1" (display the top header).
* outputsimtime - If set to "1", the simulation time is included as one of the statistics fields.  The default is to set the flag to "1" (display the simulation time).
* outputrank - if set to "1", the MPI rank is included as one of the statistics fields.  The default is to set the flag to "1" (display the MPI rank).

Below is an SST output specification that sends the statistics to a file named CsvOutput.csv.  The file will include a top header, the simulation time, and MPI rank.  

```
 sst.setStatisticOutput("sst.statOutputCSV", {"filepath": "./CsvOutput.csv",
                                              "separator": ",",
                                              "outputtopheader": "1",
                                              "outputrank": "1",
                                              "outputsimtime": "1"})
```
The contents of the file are shown below.

```
ComponentName,StatisticName,StatisticSubId,StatisticType,SimTime,Rank,Sum.u64,SumSQ.u64,Count.u64,Min.u64,Max.u64
stats_example,INT64_clock_ticks,,Accumulator,10000,0,45,285,10,0,9
```

#### Sending Statistics to a Compressed File

Statistics can also be sent to compressed text and csv files.  The associated parameters for each remain the same.  The only difference is to specify sst.statOutputTXTgz or sst.statOutputCSVgz when calling the sst.setStatisticOutput() method, as appropriate.  However, these output file types are only available if libz is included when building SST.

### Enabling the Statistics

Statistics can be enabled by component name, component type, or all at once.

#### Enable Statistics by Component Name

There are three methods available to enable statistics by component name.

* enableAllStatisticsForComponentName() - Enables all statistics for a specific component using the given statistic parameters.  The component name is that specified when creating the component in the Python configuration file.  The statistic parameters are specified using JSON as described above.  An example is given below.
```
sst.enableAllStatisticsForComponentName(componentName,
                                       {"type":"sst.AccumulatorStatistic",
                                        "rate":"1ns"})
```

* enableStatisticForComponentName() - Enables a selected statistic for a specific component.  This is similar to enableAllStatisticsForComponentName() except a statistic name is given in addition to the component name.  The statistic name is that specified when the statistic is documented using the `SST_ELI_DOCUMENT_STATISTICS` macro.  An example is given below. 
```
sst.enableStatisticForComponentName(componentName,
                                    "INT64_f_clock_ticks", 
                                   {"type":"sst.AccumulatorStatistic",
                                    "rate":"1ns"})
```

* enableStatisticsForComponentName() - Enable a list of selected statistics for a specific component.  This is similar to enableStatisticForComponentName() except a list of statistic names is given instead of a single statistic name.  An example is given below.

```
sst.enableStatisticsForComponentName(componentName,
                                    ["INT64_f_clock_ticks", 
                                     "INT64_r_clock_ticks"],
                                    {"type":"sst.AccumulatorStatistic",
                                     "rate":"1ns"})
```

#### Enable Statistics by Component Type

There are three methods available to enable statistics by component type.

* enableAllStatisticsForComponentType() - Enables all statistics for a component type using the given statistic parameters.  The component type is that used when creating the component in the Python configuration file.  The statistic parameters are specified using JSON as described above.  An example is given below.
```
sst.enableAllStatisticsForComponentType("example.StatsComponent",
                                       {"type":"sst.AccumulatorStatistic",
                                        "rate":"1ns"})
```

* enableStatisticForComponentType() - Enables a selected statistic for a specific component type.  This is similar to enableAllStatisticsForComponentType() except a statistic name as well as the component type is given.  The statistic name is that specified when the statistic is documented using the `SST_ELI_DOCUMENT_STATISTICS` macro.  An example is given below. 
```
sst.enableStatisticForComponentType("example.StatsComponent",
                                    "INT64_f_clock_ticks", 
                                   {"type":"sst.AccumulatorStatistic",
                                    "rate":"1ns"})
```

* enableStatisticsForComponentType() - Enable a list of selected statistics for a specific component type.  This is similar to enableStatisticForComponentType() except a list of statistic names is given instead of a single statistic name.  An example is given below.

```
sst.enableStatisticsForComponentType("example.StatsComponent",
                                    ["INT64_f_clock_ticks", 
                                     "INT64_r_clock_ticks"],
                                    {"type":"sst.AccumulatorStatistic",
                                     "rate":"1ns"})
```

#### Enable Statistics All At Once

To enable all statistics of all component types use the `enableAllStatisticsForAllComponents()` method.  There is no need to specify a component type or statistic name, simply specify the statistic parameters.  An example is given below.

```
sst.enableAllStatisticsForAllComponents({"type":"sst.AccumulatorStatistic",
                                         "rate":"1ns"})
```

## Examples

Defined below is a simple component containing two statistics.  The component contains a counter and each time the component's clock event handler is called the counter is incremented and its value added to the statistics.  The first statistic contains the values incrementing from 0 - 9.  The second statistic contains the values decrementing from 9 - 0.

Listing `StatsComponent.h`
```
#ifndef STATS_COMPONENT_H
#define STATS_COMPONENT_H

#include <sst/core/component.h>
#include <sst/core/output.h>
#include <sst/core/params.h>

namespace Stats
{
    // Define the log levels.  Each level displays its own message and the
    // levels above it.
    //
    // For example, ALL displays all log messages, INFO displays INFO, WARN,
    // and FATAL messages, FATAL displays only FATAL messages.
    //
    const uint64_t FATAL = 0;
    const uint64_t WARN  = 1;
    const uint64_t INFO  = 2;
    const uint64_t DEBUG = 3;
    const uint64_t TRACE = 4;
    const uint64_t ALL   = 5;

    // This is a very simple component.  It only registers a clock and 
    // adds data to two defined statistics.
    //
    class StatsComponent : public SST::Component
    {
        public:
            // Constructor/Destructor
            //
            StatsComponent(SST::ComponentId_t id, SST::Params &params);
            ~StatsComponent() {}

            // Standard SST::Component functions.  These all need to
            // be implemented in the component, even if they are empty.
            //
            void setup(void);
            void finish(void);

            // Clock handler.  This is the method called from the clock event.
            //
            bool clockTick(SST::Cycle_t cycle);

            // Shared documentation macros. 
            //
            SST_ELI_DOCUMENT_PARAMS(
                { "debug", "Debug level:  (0: NONE, 1: STDOUT, 2: STDERR, 3: FILE)", "0" },
                { "clock", "Component clock rate", "1GHz" }
            )
            
            SST_ELI_REGISTER_COMPONENT(
                StatsComponent,                         // Class name
                "statsexample",                         // Library name (the *.so)
                "StatsComponent",                       // Name used to reference the component.  This can be
                                                        // whatever you want it to be and will be referenced
                                                        // in the python configuration file.
                SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // Version number
                "Statistics example",                   // Description                 
                COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
            )

            // Document the statistics that this component provides
            // { "statistic_name", "description", "units", enable_level }
            //
            SST_ELI_DOCUMENT_STATISTICS( 
                {"INT64_f_clock_ticks", "Statistic to record the number of clock ticks, counting up", "ticks", 2},
                {"INT64_r_clock_ticks", "Statistic to record the number of clock ticks, counting down", "ticks", 2}
            )

        private:
            // Member variables for this example.
            //
            SST::Output logger_;                        // For displaying log messages.
            uint64_t componentId_;                      // SST supplied component id.
            uint64_t clockTicks_;                       // Counter to keep track of clock ticks.
            uint64_t maxClockTicks_;                    // Maximum number of clock ticks.
            Statistic<uint64_t>* forward_clock_ticks;   // Statistic to count forward clock ticks.
            Statistic<uint64_t>* reverse_clock_ticks;   // Statistic to count reverse clock ticks.

    };  // Close the class
}   // Close the namespace

#endif
```
Listing `StatsComponent.cc`
```
#include "StatsComponent.h"
#include <iostream>

using namespace Stats;

// Component constructor.
//
StatsComponent::StatsComponent(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    componentId_(id),
    clockTicks_(0),
    maxClockTicks_(10)
{
    // Read in the parameters from the python config file.  See SST_ELI_DOCUMENT_PARAMS
    // for an explanation of what each parameter represents.
    //
    std::string clock = 
        params.find<std::string>("clock", "1GHz");
    unsigned int debug = 
        params.find<int>("debug", ALL);

    // Create the logger.
    //
    logger_ = SST::Output("Time=@t; File=@f; Func=@p; Line=@l; Thread=@I -- ", debug, 0x01, SST::Output::STDOUT);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering constructor for component id %lu\n", componentId_);

    // Initialize the debug output instance.
    // Strings for debug output use the printf format.
    //
    logger_.verbose(CALL_INFO, INFO,  0x00, "Initializing component %lu.\n", id);
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Parameters successfully read from config file.\n");
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "clockTicks = %lu\n", clockTicks_);
    logger_.verbose(CALL_INFO, INFO,  0x00, "Constructing new Example Instance.\n");

    // Configure the component clock.
    //
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Clock rate is: %s\n", clock.c_str());
    registerClock(clock,
        new SST::Clock::Handler<StatsComponent>(this, &StatsComponent::clockTick));
    logger_.verbose(CALL_INFO, INFO,  0x00, "Successfully initialized clock.\n");

    // Register statistics with the component.
    //
    forward_clock_ticks = registerStatistic<uint64_t>("INT64_f_clock_ticks");
    reverse_clock_ticks = registerStatistic<uint64_t>("INT64_r_clock_ticks");

    // Register this component with the simulation.
    //
    registerAsPrimaryComponent();
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Component registered as primary component.\n");

    primaryComponentDoNotEndSim();
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Simulation notified it should not end.\n");
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving constructor for component id %lu\n", componentId_);
}


// Called after all components have been constructed and initialization
// has completed, but before simulation time has begin.
//
// This is where you should do any other initialization that needs done
// but could be accomplished in the constructure.
//
void StatsComponent::setup(void)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering setup for component id %lu\n", componentId_);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving setup for component id %lu\n", componentId_);
}


// Called after the simulation is complete but before the objects are
// destroyed.  This is a good place to print out statistics.
//
void StatsComponent::finish(void)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering finish for component id %lu\n", componentId_);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving finish for component id %lu\n", componentId_);
}


// Clock event handler.
//
bool StatsComponent::clockTick(SST::Cycle_t cycle)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering clock for component id %lu\n", componentId_);

    // Check to see if it's time to end the simulation.
    //
    bool done = (clockTicks_ == maxClockTicks_);

    logger_.verbose(CALL_INFO, INFO, 0x00, "Clock tick count = %lu out of %lu\n", clockTicks_, maxClockTicks_);
    if (done)
    {
        logger_.verbose(CALL_INFO, INFO, 0x00, "Ending sim.\n");
        primaryComponentOKToEndSim();
    }
    else
    {
        logger_.verbose(CALL_INFO, INFO, 0x00, "Adding to the clock tick statistic\n");
        forward_clock_ticks->addData(clockTicks_);
        reverse_clock_ticks->addData(maxClockTicks_ - clockTicks_ - 1);
    }

    // Update the clock tick count for the next time around.
    //
    clockTicks_ += 1;

    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}
```
Listing `Makefile`
```

CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libstatsexample.so install

libstatsexample.so: 	StatsComponent.cc\
	    				StatsComponent.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register statsexample statsexample_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libstatsexample.so
```
This component definition is the same for each example given below.  The statistics to be calculated and how they are displayed is specified in the Python configuration file.

### Printing an Accumulator to the Console

Below is Python configuration defining the statistics as accumulators and printing their contents to the console.  The statistics are printed every ns while the simulation is running.

```
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
```

The expected results are listed below.
```
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 0; SumSQ.u64 = 0; Count.u64 = 1; Min.u64 = 0; Max.u64 = 0; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 9; SumSQ.u64 = 81; Count.u64 = 1; Min.u64 = 9; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 1; SumSQ.u64 = 1; Count.u64 = 2; Min.u64 = 0; Max.u64 = 1; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 17; SumSQ.u64 = 145; Count.u64 = 2; Min.u64 = 8; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 3; SumSQ.u64 = 5; Count.u64 = 3; Min.u64 = 0; Max.u64 = 2; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 24; SumSQ.u64 = 194; Count.u64 = 3; Min.u64 = 7; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 6; SumSQ.u64 = 14; Count.u64 = 4; Min.u64 = 0; Max.u64 = 3; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 30; SumSQ.u64 = 230; Count.u64 = 4; Min.u64 = 6; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 10; SumSQ.u64 = 30; Count.u64 = 5; Min.u64 = 0; Max.u64 = 4; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 35; SumSQ.u64 = 255; Count.u64 = 5; Min.u64 = 5; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 15; SumSQ.u64 = 55; Count.u64 = 6; Min.u64 = 0; Max.u64 = 5; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 39; SumSQ.u64 = 271; Count.u64 = 6; Min.u64 = 4; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 21; SumSQ.u64 = 91; Count.u64 = 7; Min.u64 = 0; Max.u64 = 6; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 42; SumSQ.u64 = 280; Count.u64 = 7; Min.u64 = 3; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 28; SumSQ.u64 = 140; Count.u64 = 8; Min.u64 = 0; Max.u64 = 7; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 44; SumSQ.u64 = 284; Count.u64 = 8; Min.u64 = 2; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 36; SumSQ.u64 = 204; Count.u64 = 9; Min.u64 = 0; Max.u64 = 8; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 9; Min.u64 = 1; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9; 
 stats_example.INT64_f_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9; 
 stats_example.INT64_r_clock_ticks : Accumulator : Sum.u64 = 45; SumSQ.u64 = 285; Count.u64 = 10; Min.u64 = 0; Max.u64 = 9; 
Simulation is complete, simulated time: 11 ns
```
### Printing a Histogram to a CSV File

Below is Python configuration defining the statistics as histograms and printing their contents to a CSV file.  The statistics are printed every ns while the simulation is running.
```
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
sst.enableAllStatisticsForAllComponents({"type":"sst.HistogramStatistic",
                                         "rate":"1ns",
                                         "minvalue":"0",
                                         "binwidth":"5",
                                         "numbins":"2"}); 
```
The expected results are listed below.  The values for each statistic are interspersed so further analysis of an individual statistic will require the file values to be parsed in some manner.

```
ComponentName, StatisticName, StatisticSubId, StatisticType, SimTime, Rank, BinsMinValue.u64, BinsMaxValue.u64, BinWidth.u64, TotalNumBins.u32, Sum.u64, SumSQ.u64, NumActiveBins.u32, NumItemsCollected.u64, NumItemsBinned.u64, NumOutOfBounds-MinValue.u64, NumOutOfBounds-MaxValue.u64, Bin0:0-5.u64, Bin1:5-10.u64
stats_example, INT64_f_clock_ticks, , Histogram, 1000, 0, 0, 9, 5, 2, 0, 0, 1, 1, 1, 0, 0, 1, 0
stats_example, INT64_r_clock_ticks, , Histogram, 1000, 0, 0, 9, 5, 2, 9, 81, 1, 1, 1, 0, 0, 0, 1
stats_example, INT64_f_clock_ticks, , Histogram, 2000, 0, 0, 9, 5, 2, 1, 1, 1, 2, 2, 0, 0, 2, 0
stats_example, INT64_r_clock_ticks, , Histogram, 2000, 0, 0, 9, 5, 2, 17, 145, 1, 2, 2, 0, 0, 0, 2
stats_example, INT64_f_clock_ticks, , Histogram, 3000, 0, 0, 9, 5, 2, 3, 5, 1, 3, 3, 0, 0, 3, 0
stats_example, INT64_r_clock_ticks, , Histogram, 3000, 0, 0, 9, 5, 2, 24, 194, 1, 3, 3, 0, 0, 0, 3
stats_example, INT64_f_clock_ticks, , Histogram, 4000, 0, 0, 9, 5, 2, 6, 14, 1, 4, 4, 0, 0, 4, 0
stats_example, INT64_r_clock_ticks, , Histogram, 4000, 0, 0, 9, 5, 2, 30, 230, 1, 4, 4, 0, 0, 0, 4
stats_example, INT64_f_clock_ticks, , Histogram, 5000, 0, 0, 9, 5, 2, 10, 30, 1, 5, 5, 0, 0, 5, 0
stats_example, INT64_r_clock_ticks, , Histogram, 5000, 0, 0, 9, 5, 2, 35, 255, 1, 5, 5, 0, 0, 0, 5
stats_example, INT64_f_clock_ticks, , Histogram, 6000, 0, 0, 9, 5, 2, 15, 55, 2, 6, 6, 0, 0, 5, 1
stats_example, INT64_r_clock_ticks, , Histogram, 6000, 0, 0, 9, 5, 2, 39, 271, 2, 6, 6, 0, 0, 1, 5
stats_example, INT64_f_clock_ticks, , Histogram, 7000, 0, 0, 9, 5, 2, 21, 91, 2, 7, 7, 0, 0, 5, 2
stats_example, INT64_r_clock_ticks, , Histogram, 7000, 0, 0, 9, 5, 2, 42, 280, 2, 7, 7, 0, 0, 2, 5
stats_example, INT64_f_clock_ticks, , Histogram, 8000, 0, 0, 9, 5, 2, 28, 140, 2, 8, 8, 0, 0, 5, 3
stats_example, INT64_r_clock_ticks, , Histogram, 8000, 0, 0, 9, 5, 2, 44, 284, 2, 8, 8, 0, 0, 3, 5
stats_example, INT64_f_clock_ticks, , Histogram, 9000, 0, 0, 9, 5, 2, 36, 204, 2, 9, 9, 0, 0, 5, 4
stats_example, INT64_r_clock_ticks, , Histogram, 9000, 0, 0, 9, 5, 2, 45, 285, 2, 9, 9, 0, 0, 4, 5
stats_example, INT64_f_clock_ticks, , Histogram, 10000, 0, 0, 9, 5, 2, 45, 285, 2, 10, 10, 0, 0, 5, 5
stats_example, INT64_r_clock_ticks, , Histogram, 10000, 0, 0, 9, 5, 2, 45, 285, 2, 10, 10, 0, 0, 5, 5
stats_example, INT64_f_clock_ticks, , Histogram, 11000, 0, 0, 9, 5, 2, 45, 285, 2, 10, 10, 0, 0, 5, 5
stats_example, INT64_r_clock_ticks, , Histogram, 11000, 0, 0, 9, 5, 2, 45, 285, 2, 10, 10, 0, 0, 5, 5
stats_example, INT64_f_clock_ticks, , Histogram, 11000, 0, 0, 9, 5, 2, 45, 285, 2, 10, 10, 0, 0, 5, 5
stats_example, INT64_r_clock_ticks, , Histogram, 11000, 0, 0, 9, 5, 2, 45, 285, 2, 10, 10, 0, 0, 5, 5
```

### Accumulator and Histogram Printed to a CSV File

The final example is intended to demonstrate that components can be associated with different types of statistics.  In this case, the first statistic is defined as an accumulator, the second as a histogram.  The results are printed to a CSV file.
```
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
```

The expected results are listed below.  The values for each statistic are interspersed so further analysis of an individual statistic will require the file values to be parsed in some manner.  In addition, the statistic name has not been included in the results.  Therefore, some knowledge of the statistic type will be required when parsing data for individual statistics. 

```
ComponentName, StatisticName, StatisticSubId, StatisticType, SimTime, Rank, Sum.u64, SumSQ.u64, Count.u64, Min.u64, Max.u64, BinsMinValue.u64, BinsMaxValue.u64, BinWidth.u64, TotalNumBins.u32, NumActiveBins.u32, NumItemsCollected.u64, NumItemsBinned.u64, NumOutOfBounds-MinValue.u64, NumOutOfBounds-MaxValue.u64, Bin0:0-5.u64, Bin1:5-10.u64
stats_example, , , Accumulator, 1000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 1000, 0, 9, 81, 0, 0, 0, 0, 9, 5, 2, 1, 1, 1, 0, 0, 0, 1
stats_example, , , Accumulator, 2000, 0, 1, 1, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 2000, 0, 17, 145, 0, 0, 0, 0, 9, 5, 2, 1, 2, 2, 0, 0, 0, 2
stats_example, , , Accumulator, 3000, 0, 3, 5, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 3000, 0, 24, 194, 0, 0, 0, 0, 9, 5, 2, 1, 3, 3, 0, 0, 0, 3
stats_example, , , Accumulator, 4000, 0, 6, 14, 4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 4000, 0, 30, 230, 0, 0, 0, 0, 9, 5, 2, 1, 4, 4, 0, 0, 0, 4
stats_example, , , Accumulator, 5000, 0, 10, 30, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 5000, 0, 35, 255, 0, 0, 0, 0, 9, 5, 2, 1, 5, 5, 0, 0, 0, 5
stats_example, , , Accumulator, 6000, 0, 15, 55, 6, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 6000, 0, 39, 271, 0, 0, 0, 0, 9, 5, 2, 2, 6, 6, 0, 0, 1, 5
stats_example, , , Accumulator, 7000, 0, 21, 91, 7, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 7000, 0, 42, 280, 0, 0, 0, 0, 9, 5, 2, 2, 7, 7, 0, 0, 2, 5
stats_example, , , Accumulator, 8000, 0, 28, 140, 8, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 8000, 0, 44, 284, 0, 0, 0, 0, 9, 5, 2, 2, 8, 8, 0, 0, 3, 5
stats_example, , , Accumulator, 9000, 0, 36, 204, 9, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 9000, 0, 45, 285, 0, 0, 0, 0, 9, 5, 2, 2, 9, 9, 0, 0, 4, 5
stats_example, , , Accumulator, 10000, 0, 45, 285, 10, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 10000, 0, 45, 285, 0, 0, 0, 0, 9, 5, 2, 2, 10, 10, 0, 0, 5, 5
stats_example, , , Accumulator, 11000, 0, 45, 285, 10, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 11000, 0, 45, 285, 0, 0, 0, 0, 9, 5, 2, 2, 10, 10, 0, 0, 5, 5
stats_example, , , Accumulator, 11000, 0, 45, 285, 10, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
stats_example, , , Histogram, 11000, 0, 45, 285, 0, 0, 0, 0, 9, 5, 2, 2, 10, 10, 0, 0, 5, 5
```
## Final Notes

As of SST V10 there are a few idiosyncrasies to keep in mind when working with statistics in SST.  The noted items may explain unusual behavior seen when using statistics but will not prevent using them.

1.  For histograms, the minimum bin value must be set to zero.  Otherwise, the values will not be binned properly.

2.  There may be some difficulty using the startat statistics parameter.  If set to any value other than 0 statistics will not be enabled even though entries will be printed at the specified rate.

3.  There may be some difficulty using the stopat statistics parameter.  Statistics will not be disabled when the simulation time reaches the specified value.

4.  When sending statistic outputs to a text file with the outputtopheader flag enabled, the included top header will have the column names concatenated multiple times.  Therefore, it's advised the option to display the column names inline be used instead.  If it's desired to have an output file with a top level header, use the CSV file output.
