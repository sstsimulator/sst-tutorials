# Communicating Between Two Components

Communication between components in SST is accomplished by passing events over links that connect the components.  This example will demonstrate how to send messages between two components in SST.  To show that the components are really communicating the component clock event handler will be modified to increment the clock tick count when a message is received and end the simulation when the specified number of clock counts have been received.

For this example we'll need to make the following changes.

1.  Add a link to the component over which the message will be sent.
2.  Define the message to be sent.
3.  Update the clock event handler to process the incoming messages.
4.  Configure the simulation to connect two components via the link.


## Example Setup

To set up for this example, perform the following steps:

1.  Navigate to the `example` folder created in `Example00` and create a folder named `Example01`.
2.  Copy the files created as part of `Example00` \(`ExampleComponent.cc`, `ExampleComponent.h`, `ExampleConfig.py`, and `Makefile`\) to the `Example01` folder.
3.  Open the copied `ExampleComponent.h` file and change the namespace to `Example01`.
4.  Open the copied `ExampleComponent.cc` file and change the `using namespace` directive to refer to the `Example01` namespace.

## Adding the Link

An SST link is an object within an SST component over which messages are sent.  Its name is a bit of a misnomer since a link is normally thought of as the actual connection between two objects, like the telephone line between two houses or the network cable between two computers.  In fact, the SST code interface and documentation seems to use the names "link" and "port" interchangeably.  For this example, the term "link" will be used for consistency unless the SST interface specifically uses the term "port".

To add a link to the component definition, open the file `ExampleComponent.h`.  At the top of the file, add the SST link include file.

```
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/output.h>
#include <sst/core/params.h>
```

Then add the link definition following the `setup()` and `finish()` method definitions.

```
            // Standard SST::Component functions.  These all need to
            // be implemented in the component, even if they are empty.
            //
            void setup(void);
            void finish(void);

            // Links to connect to other component.
            // The link connects to the components port.
            //
            SST::Link* link_;
```

Finally, use the `SST_ELI_DOCUMENT_PORTS` macro to define the link in the ELI database.  Notice, this is one of the places where the SST code interface refers to a link as a port.  Place this macro below the previously defined macros.

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
                "ExampleComponent",                     // Named use to reference the component.  This can be
                                                        // whatever you want it to be and will be referenced
                                                        // in the python configuration file.
                SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // Version number
                "Clock with port element example",      // Description                 
                COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
            )

            SST_ELI_DOCUMENT_PORTS(
                { "link_", "Single port", {}}
            )
```

In the `SSE_ELI_DOCUMENT_PORTS` macro definition the first parameter \("link_"\) is the name assigned to the documented port.  It does not have to match the variable name, this has just been done that to make the connection between the two explicit.  This name will be used later to reference the link in the simulation configuration file.  

The second parameter is a description and can be anything.  The third parameter is not used for this example.

Save this file and open the `ExampleComponent.cc` file where the component functionality is defined.  

In the component constructor, add the code to configure the link above that portion where the component clock is configured

```
    // Configure the links (connections to other components).
    // The link is associated with a component's registered port.
    // No handler for this example.
    //
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Configuring link.\n");
    link_ = configureLink("link_");

    // Configure the component clock.
    //
    logger_.verbose(CALL_INFO, INFO,  0x00, "Clock rate is: %s\n", clock.c_str());
    registerClock(clock,
        new SST::Clock::Handler<ExampleComponent>(this, &ExampleComponent::clockTick));
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Successfully initialized clock.\n");
```

and save the file.  At this point, adding the link to the component is complete.

## Defining the Message to be Sent

Messages in SST are defined as events.  In the editor of your choice, create a new file.  Enter the code shown below and save it as ExampleEvent.h.

```
#ifndef _EXAMPLE_EVENT_H
#define _EXAMPLE_EVENT_H

// Basic include files from SST
//
#include <sst/core/event.h>

namespace Example01
{
    // Class must inherit from SST::Event
    //
    class ExampleEvent : public SST::Event
    {
    public:
        // Constructor
        //
        ExampleEvent() : SST::Event() {}

        // Return the payload
        //
        size_t getPayload() const { return 0; }

    private:

        // Serialization code.  Serializes data before sending it out
        // over the link.
        //
        void serialize_order(SST::Core::Serialization::serializer &ser) override
        {
            Event::serialize_order(ser);
        }
        ImplementSerializable(ExampleEvent);
    };
}

#endif
```

Similar to how components are defined, the first line

```
#include <sst/core/event.h>
```

specifies the include file needed from the SST core libraries.

Define the namespace

```
namespace Example01
{
```

and define the class

```
    // Class must inherit from SST::Event
    //
    class ExampleEvent : public SST::Event
    {
```

Just as components are required to inherit from the class `SST::Component`, events are required to inhert from `SST::Event` or one of its children.

A constructor is defined.  However, it's empty since there is no initialization that needs to take place.  In addition, a method to return the event payload is defined.  However, since this event contains no payload it just returns 0.

```
        // Constructor
        //
        ExampleEvent() : SST::Event() {}

        // Return the payload
        //
        size_t getPayload() const { return 0; }
```

The serialization method is not used for this example and will be explained later.

## Update the Clock Handler

The updated clock handler is contained in the `ExampleComponent.cc` file.  Open this file in the editor of your choice and replace the `clockTick` method with the code listed below.

```
// Clock event handler.
//
bool ExampleComponent::clockTick(SST::Cycle_t cycle)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering clock for component id %lu\n", componentId_);
    bool done = false;

    // Poll the link for incoming messages and process them as necessary.
    //
    for (;;)
    {
        logger_.verbose(CALL_INFO, DEBUG, 0x00, "Retreiving event from link in component id %lu\n", componentId_);
        ExampleEvent* ev = static_cast<ExampleEvent*>(link_->recv());

        if (nullptr == ev)
        {
            logger_.verbose(CALL_INFO, DEBUG, 0x00, "No event available from link in component id %lu\n", componentId_);
            break;
        }
        else
        {
            // Increment the clock tick counter and end when you get to
            // the specified value.  Also, we're done with the event so
            // you can delete it.
            //
            clockTickCount_+= 1;
            done = (clockTickCount_ == clockTicks_);
            logger_.verbose(CALL_INFO, INFO, 0x00, "Clock tick count = %lu out of %lu\n", clockTickCount_, clockTicks_);
            delete ev;

            if (done)
            {
                logger_.verbose(CALL_INFO, INFO, 0x00, "Ending sim.\n");
                primaryComponentOKToEndSim();
                break;
            }
        }
    }

    // Send an event over the link.
    //
    ExampleEvent* ev = new ExampleEvent();
    logger_.verbose(CALL_INFO, INFO, 0x00, "Sending event over the link.\n");
    link_->send(ev);

    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}
```

The updated clock event handler consists of two parts.  First the link is polled to see if there are any incoming messages.  If so, the `clockTickCount` variable is incremented and, when the desired number of messages are received, `primaryComponentOKToSendSim()` is called to notify SST that, as far as this component is concerned, it's ok to end the simulation.

After any incoming messages are processed, a message is sent from this component over the link to the connected component, which will process incoming messages in the same manner.  Eventually both components will notify SST it's ok to end the simulation and the simulation will end.

Finally, add a reference to the `ExampleEvent.h` include file at the top of the file.

```
#include "ExampleComponent.h"
#include "ExampleEvent.h"
#include <iostream>
```

## Configuring the Simulation

The updated simulation configuration file is listed below.

```
# Execute from the command line with the command:
#   sst exampleConfig.py 2>&1 | tee test.log
#
import sst

# Initialize local variables.
#
clockTicks = "5"
clock = "1GHz"
debug = "2"     #0 = FATAL, 1 = WARN, 2 = INFO, 3 = DEBUG, 4 = TRACE, 5 = ALL

componentName0 = "example_00"
componentName1 = "example_01"

# Define the component.
#
# The parameters are a dictionary and can be any key/value pair defined
# by the component itself.
#
# The second parameter is <library>.<registered_name>
#
obj0 = sst.Component(componentName0, "example.ExampleComponent")
obj0.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
    })

obj1 = sst.Component(componentName1, "example.ExampleComponent")
obj1.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
})

# Connect the objects to each other.
#
link = sst.Link("link0")
link.connect((obj0, "link_", "5ns"), (obj1, "link_",  "5ns"))

```

For the most part, it's the same as that from `Example00`, except for the addition of a second component and establishment of a link between the two.

The link is established using the link `connect` method.  The `connect` method takes two parameters, each of them a tuple containing values used to configure the link coming from a component.  

The first value in the tuple is the component connecting to the link.  In this case, a link is being established between components obj0 and obj1.

The second value in the tuple is the identifier used to refer to the link within the component.  This is the same value as that defined in the `SST_ELI_DOCUMENT_PORTS` macro in the component include file.

The final value in the tuple is the link delay.  This specifies how long it will take the message to traverse the link from the corresponding component and must be greater than 0.  The implications of this value will be discussed below.

## Building the Simulation

The updated make file is listed below.

```

CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libexample.so install

libexample.so: ExampleComponent.cc\
               ExampleComponent.h\
			   ExampleEvent.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register example example_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libexample.so
```

It is identical to that of Example00 except for the addition of the file `ExampleEvent.h` in the list of source files.

Open a terminal, navigate to the Example01 folder, and execute the command

```
make clean; make
```

If there are errors, examine the source files to find the error in the source code an try again.

## Executing the Simulation

Once the simulation builds correctly, it can be executed using the command

```
sst ExampleConfig.py
```
For this example, the output below will be displayed.

```
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=34; Thread=0 -- Initializing component 0.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=48; Thread=0 -- Clock rate is: 1GHz
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=34; Thread=0 -- Initializing component 1.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=48; Thread=0 -- Clock rate is: 1GHz
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 1 out of 5
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 1 out of 5
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 2 out of 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 2 out of 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 3 out of 5
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 3 out of 5
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 4 out of 5
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 4 out of 5
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 5 out of 5
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=118; Thread=0 -- Ending sim.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=113; Thread=0 -- Clock tick count = 5 out of 5
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=118; Thread=0 -- Ending sim.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event over the link.
Simulation is complete, simulated time: 10 ns
```

One thing you may notice is that even though the configuration file indicated the components should only count 5 incoming messages, it took 10 clock cycles to complete.  This is where the effect of the link delay becomes apparent.  When configuring the links a delay of 5ns was specified for each.  This means a message will not be processed in a connected node until more than 5 ns have elapsed in the simulation.  If you examine the output you'll see that is the case.

In fact, while the same delay was specified in each direction for this run, that is not necessary.  Below is the results of a simulation run where the delay in one direction was specified as 5ns, the delay in the other direction was specified as 2ns.  If you examine the output you'll see one of the components starts receiving messages once 3ns have elapsed, while other component does not begin receiving messages until 6ns have elapsed.

```
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=34; Thread=0 -- Initializing component 0.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=48; Thread=0 -- Clock rate is: 1GHz
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=34; Thread=0 -- Initializing component 1.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=48; Thread=0 -- Clock rate is: 1GHz
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 1 out of 5
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 2 out of 5
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 3 out of 5
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 4 out of 5
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 1 out of 5
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 5 out of 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Ending sim.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 2 out of 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 3 out of 5
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 4 out of 5
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=109; Thread=0 -- Clock tick count = 5 out of 5
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Ending sim.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=119; Thread=0 -- Sending event over the link.
Simulation is complete, simulated time: 10 ns
```

## Summary

This example demonstrates how to send messages between two components in an SST simulation.  However, messages are not really useful unless they contain information.  Including and processing that information in a message is the subject of the next example.
