# The Simplest Simulation

All SST simulations consist of components and subcomponents that pass messages over connecting links.  So at its most basic, the simplest possible SST simulation consists of a single component that performs some action.

This is the first example to be implemented, a single component that will count to a specified limit and end.

Navigate to your top level SST folder and create a folder named `example`.  Then navigate to the folder you just created and create another folder named `Example00`.  This folder will contain all the files necessary to implement the simulation.

## Defining a Component

Navigate to the Example00 folder.  In the editor of your choice, create a new file and enter the code show below.  Save it as `ExampleComponent.h`.

```
#ifndef MY_COMPONENT_H
#define MY_COMPONENT_H

#include <sst/core/component.h>
#include <sst/core/output.h>
#include <sst/core/params.h>

namespace Example00
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
    // prints log messages as the clock handler is called.
    //
    // Remember, all components inherit from SST::Component
    //
    class ExampleComponent : public SST::Component
    {
        public:
            // Constructor/Destructor
            //
            ExampleComponent(SST::ComponentId_t id, SST::Params &params);
            ~ExampleComponent() {}

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
                { "debug", "Debug location:  (0: NONE, 1: STDOUT, 2: STDERR, 3: FILE)", "0" },
                { "clock", "Component clock rate", "1GHz" },
                { "clockTicks", "Number of times the handler is called before ending.", "10" }
            )
            
            SST_ELI_REGISTER_COMPONENT(
                ExampleComponent,                       // Class name
                "example",                              // Library name (the *.so)
                "ExampleComponent",                     // Name used to reference the component.  This can be
                                                        // whatever you want it to be and will be referenced
                                                        // in the python configuration file.
                SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // Version number
                "Clock element example",                // Description                 
                COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
            )

        private:
            // Member variables for this example.
            //
            SST::Output logger_;        // For displaying log messages.
            uint64_t componentId_;      // SST supplied component id.
            uint64_t clockTicks_;       // Maximum number of clock ticks.
            uint64_t clockTickCount_;   // Clock ticks counter.

    };  // Close the class
}   // Close the namespace

#endif
```

This code defines the component used in the simulation.  The first few lines

```
#include <sst/core/component.h>
#include <sst/core/output.h>
#include <sst/core/params.h>
```

specify include files needed from the SST core libraries.  The file `component.h` defines the SST base component from which all SST components are derived.  The file `output.h` contains definitions used for logging.  The file `params.h` contains definitions used to process incoming parameters from the simulation configuration file, which will be described later.

The logging levels defined below the include files are for convenience when writing log messages.

Next the namespace is defined.

```
namespace Example00
{
```

This is not strictly necessary but is useful to prevent name clashes.

Finally the component is defined.

```
    // This is a very simple component.  It only registers a clock and 
    // prints log messages as the clock handler is called.
    //
    // Remember, all components inherit from SST::Component
    //
    class ExampleComponent : public SST::Component
    {
        public:
            // Constructor/Destructor
            //
            ExampleComponent(SST::ComponentId_t id, SST::Params &params);
            ~ExampleComponent() {}

            // Standard SST::Component functions.  These all need to
            // be implemented in the component, even if they are empty.
            //
            void setup(void);
            void finish(void);

            // Clock handler.  This is the method called from the clock event.
            //
            bool clockTick(SST::Cycle_t cycle);
```

As noted in the comments, all SST components are derived from the base component SST::Component or one of its children.  The constructor and destructor are a standard format and required to interface with the SST core functions.  

The `setup()` and `finish()` methods are also required to interface with the SST core functions.  `setup()` is called after the component constructor but before the simulation starts.  As such, it's a good place to put any component initialization that can't be done in the constructor.  `finish()` is called after the simulation ends and it a good place to perform any necessary cleanup associated with the component.

As for the clock event handler, when configuring the component in the previously mentioned simulation configuration file, the component will be assigned a clock rate.  On each clock tick, the clock event handler is called.  This makes the clock event handler a good place to put code that needs to be executed on a periodic basis.

The component is described to the SST core using the SST_ELI macros.

```
            // Shared documentation macros. 
            //
            SST_ELI_DOCUMENT_PARAMS(
                { "debug", "Debug location:  (0: NONE, 1: STDOUT, 2: STDERR, 3: FILE)", "0" },
                { "clock", "Component clock rate", "1GHz" },
                { "clockTicks", "Number of times the handler is called before ending.", "10" }
            )
            
            SST_ELI_REGISTER_COMPONENT(
                ExampleComponent,                       // Class name
                "example",                              // Library name (the *.so)
                "ExampleComponent",                     // Name used to reference the component.  This can be
                                                        // whatever you want it to be and will be referenced
                                                        // in the python configuration file.
                SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // Version number
                "Clock element example",                // Description                 
                COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
            )
```

The `SST_ELI_DOCUMENT_PARAMS` macro defines the component parameters and their default values.  The `SST_ELI_REGISTER_COMPONENT` macro registers the component with the ELI database, making it available for use by the simulation configuration file.

Finally, a few private variables that will be used to track state within the component are defined.

```
        private:
            // Member variables for this example.
            //
            SST::Output logger_;        // For displaying log messages.
            uint64_t componentId_;      // SST supplied component id.
            uint64_t clockTicks_;       // Maximum number of clock ticks.
            uint64_t clockTickCount_;   // Clock ticks counter.
```

## Implementing Component Functionality

To implement the component functionality, use the editor of your choice and create a new file.  Enter the code shown below and save it as `ExampleComponent.cc`.

```
#include "ExampleComponent.h"
#include <iostream>

using namespace Example00;

// Component constructor.
//
ExampleComponent::ExampleComponent(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    componentId_(id),
    clockTickCount_(0)
{
    // Read in the parameters from the python config file.  See SST_ELI_DOCUMENT_PARAMS
    // for an explanation of what each parameter represents.
    //
    std::string clock = 
        params.find<std::string>("clock", "1GHz");
    clockTicks_ = static_cast<uint64_t>(
        params.find<int>("clockTicks", 10));
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
        new SST::Clock::Handler<ExampleComponent>(this, &ExampleComponent::clockTick));
    logger_.verbose(CALL_INFO, INFO,  0x00, "Successfully initialized clock.\n");

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
void ExampleComponent::setup(void)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering setup for component id %lu\n", componentId_);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving setup for component id %lu\n", componentId_);
}


// Called after the simulation is complete but before the objects are
// destroyed.  This is a good place to print out statistics.
//
void ExampleComponent::finish(void)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering finish for component id %lu\n", componentId_);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving finish for component id %lu\n", componentId_);
}


// Clock event handler.
//
bool ExampleComponent::clockTick(SST::Cycle_t cycle)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering clock for component id %lu\n", componentId_);

    // Increment the clock tick counter and end when you get to
    // the specified value.
    //
    clockTickCount_ += 1;
    bool done = (clockTickCount_ == clockTicks_);

    logger_.verbose(CALL_INFO, INFO, 0x00, "Clock tick count = %lu out of %lu\n", clockTickCount_, clockTicks_);
    if (done)
    {
        logger_.verbose(CALL_INFO, INFO, 0x00, "Ending sim.\n");
        primaryComponentOKToEndSim();
    }
   
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}
```

### Understanding the Constructor

For this component, all initialization takes place in the constructor.

First, the component parameters are read from the configuration file.

```
   // Read in the parameters from the python config file.  See SST_ELI_DOCUMENT_PARAMS
    // for an explanation of what each parameter represents.
    //
    std::string clock = 
        params.find<std::string>("clock", "1GHz");
    clockTicks_ = static_cast<uint64_t>(
        params.find<int>("clockTicks", 10));
    unsigned int debug = 
        params.find<int>("debug", ALL);
```

The component parameters were previously listed as part of the `SST_ELI_DOCUMENT_PARAMS` macro and have their values set in the simulation configuration file.  For this component, the parameters consists of the clock rate, number of clock ticks, and the debug level.  The number of clock ticks specifies the number of clock periods that elapse before the simulation ends.  The debug level provides a way to filter which debug messages are displayed and are listed in the component definition file.

Next the logger is created.  For more information on how the logger is configured and used, consult the SST documentation.

```
    // Create the logger.
    //
    logger_ = SST::Output("Time=@t; File=@f; Func=@p; Line=@l; Thread=@I -- ", debug, 0x01, SST::Output::STDOUT);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering constructor for component id %lu\n", componentId_);

```

After the logger, the component clock is configured.  This simply consists of registering the clock and specifying the clock event handler.  The clock event handler is called each clock period and is discussed below.

```
    // Configure the component clock.
    //
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Clock rate is: %s\n", clock.c_str());
    registerClock(clock,
        new SST::Clock::Handler<ExampleComponent>(this, &ExampleComponent::clockTick));
    logger_.verbose(CALL_INFO, INFO,  0x00, "Successfully initialized clock.\n");
```

Finally the component is registered with the simulation.

```
    // Register this component with the simulation.
    //
    registerAsPrimaryComponent();
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Component registered as primary component.\n");
    primaryComponentDoNotEndSim();
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Simulation notified it should not end.\n");
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving constructor for component id %lu\n", componentId_);
```

Registering the component as a primary component allows this component to specify when it is ok to end the simulation through the use of the `primaryComponentDoNotEndSim()` and `primaryComponentOKToEndSim()` calls.  The simulation will not end naturally until all primary components specify it is ok to end the simulation.

### Understanding setup() and finish()

As described previously, `setup()` and `finish()` are where additional component initialization and cleanup take place.  They are called prior to the simulation starting and after the simulation ends, respectively.  In this case, there is no initialization or cleanup needed so they simply contain logging statements to indicate they were called.

```
// Called after all components have been constructed and initialization
// has completed, but before simulation time has begin.
//
// This is where you should do any other initialization that needs done
// but could be accomplished in the constructure.
//
void ExampleComponent::setup(void)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering setup for component id %lu\n", componentId_);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving setup for component id %lu\n", componentId_);
}


// Called after the simulation is complete but before the objects are
// destroyed.  This is a good place to print out statistics.
//
void ExampleComponent::finish(void)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering finish for component id %lu\n", componentId_);
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving finish for component id %lu\n", componentId_);
}
```

### Understanding the Clock Event Handler

The clock event handler is a method specifed in the component constructor and called on each tick of the component clock.

```
// Clock event handler.
//
bool ExampleComponent::clockTick(SST::Cycle_t cycle)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering clock for component id %lu\n", componentId_);

    // Increment the clock tick counter and end when you get to
    // the specified value.
    //
    clockTickCount_ += 1;
    bool done = (clockTickCount_ == clockTicks_);

    logger_.verbose(CALL_INFO, INFO, 0x00, "Clock tick count = %lu out of %lu\n", clockTickCount_, clockTicks_);
    if (done)
    {
        logger_.verbose(CALL_INFO, INFO, 0x00, "Ending sim.\n");
        primaryComponentOKToEndSim();
    }
   
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}
```

For this simulation, each time the clock event handler is called the `clockTickCount_` value is incremented and a message sent to the logger.  When `clockTickCount_` reaches the number of clock ticks specified by the `clockTicks_` parameter, the simulation is notifed it is ok to end by calling `primaryComponentOKToEndSim()`.

## Building the Component

To build the component, use the editor of your choice and create a new file.  Enter the code show below and save it as Makefile (no extension).

```
CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libexample.so install

libexample.so: ExampleComponent.cc\
               ExampleComponent.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register example example_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libexample.so
```

Open a terminal and navigate to the folder containing the component and makefile.  Execute the command

```
make clean; make
```

If all goes well, the component will be built and installed for use by SST.  
If there are errors, go back and check to make sure the code listed above was entered correctly.

To confirm the registration worked, run

```
sst-info
```

which should yield output like

```
ELEMENT 0 = example ()
Num Components = 1
      Component 0: ExampleComponent
      CATEGORY: UNCATEGORIZED COMPONENT
         NUM STATISTICS = 0
         NUM PORTS = 0
         NUM SUBCOMPONENT SLOTS = 0
         NUM PARAMETERS = 3
            PARAMETER 0 = debug (Debug location:  (0: NONE, 1: STDOUT, 2: STDERR, 3: FILE)) [0]
            PARAMETER 1 = clock (Component clock rate) [1GHz]
            PARAMETER 2 = clockTicks (Number of times the handler is called before ending.) [10]
    ExampleComponent: Clock element example
    Using ELI version 0.9.0
    Compiled on: Nov 26 2020 15:19:14, using file: ExampleComponent.h
Num SubComponents = 0
Num Modules = 0
Num SSTPartitioners = 0
```

## Running the Simulation

To run the simulation you first have to create a simulation configuration file.  Using the editor of your choice, create a new file.  Enter the code below and save it as ExampleConfig.py.

```
# Execute from the command line with the command:
#   sst Example00Config.py 2>&1 | tee test.log
#
import sst

# Initialize local variables.
#
clockTicks = "10"   # Number of clock ticks before the simulation ends
clock = "1GHz"      # Simulation clock rate
debug = "2"         # debug level
                    # 0 = FATAL, 1 = WARN, 2 = INFO, 3 = DEBUG, 4 = TRACE, 5 = ALL

componentName0 = "example00"

# Define the component.
#
# The parameters are a dictionary and can be any key/value pair defined
# by the component itself.
#
# The second parameter is <library>.<registered_name>
# These correspond to the second and third parameters of the
# SST_ELI_REGISTER_COMPONENT macro in Example00Component.h,
# respectively.
#
obj = sst.Component(componentName0, "example.ExampleComponent")
obj.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
    })
```
The configuration file is based on the Python language so any valid Python construct can be used within the configuration file.

Because the simulation contains only a single component, the configuration file is very simple.  The top portion will be common to all SST simulation configuration files.  The sst package is imported and any local variables to be used within the configuration file are defined and initialized.

```
import sst

# Initialize local variables.
#
clockTicks = "10"   # Number of clock ticks before the simulation ends
clock = "1GHz"      # Simulation clock rate
debug = "2"         # debug level
                    # 0 = FATAL, 1 = WARN, 2 = INFO, 3 = DEBUG, 4 = TRACE, 5 = ALL
```

Then we define the component and its parameters.

```
# Define the component.
#
# The parameters are a dictionary and can be any key/value pair defined
# by the component itself.
# 
# The first parameter is the component assigned name.  This can be 
# anything that makes sense.
#
# The second parameter is <library>.<registered_name>
# These correspond to the second and third parameters of the
# SST_ELI_REGISTER_COMPONENT macro in Example00Component.h,
# respectively.
#
obj = sst.Component("example00", "example00.Example00Component")
obj.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
    })
```

You may notice the parameter values correspond to the local variables defined at the top of the configuration file.

Once the configuration file is ready, open a terminal and navigate to the folder containing the simulation code.  Execute the command

```
sst ExampleConfig.py
```

and you will get the following output, indicating the simulation ran successfully.

```
WARNING: Building component "example00" with no links assigned.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=33; Thread=0 -- Initializing component 0.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=36; Thread=0 -- Constructing new Example Instance.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=43; Thread=0 -- Successfully initialized clock.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 1 out of 10
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 2 out of 10
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 3 out of 10
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 4 out of 10
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 5 out of 10
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 6 out of 10
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 7 out of 10
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 8 out of 10
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 9 out of 10
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=90; Thread=0 -- Clock tick count = 10 out of 10
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=93; Thread=0 -- Ending sim.
Simulation is complete, simulated time: 10 ns
```

You may notice the time increments in steps of 1000.  This is because the configuration file specifies a clock of 1 GHz (1 nanosecond period), while SST operates with an internal clock of 1000 GHz (1 picosecond period) and 1 nanosecond = 1000 picoseconds.  To confirm this is the cause, change the clock in the configuration file to 10 GHz (100 picosecond period) and rerun the simulation.  Time will increment in steps of 100.  You can also change the value of `clockTicks` to get fewer or more printed lines, or change the debug level to print more or less information.

## Summary

At this point the simplest possible SST simulation consisting of a single component that prints a message on each click tick has been created.  Its purpose is to show how to specify a component, build it, and create a working simulation configuration file.  Next, we'll expand on this by creating a second component and showing how to pass messages between the two.
