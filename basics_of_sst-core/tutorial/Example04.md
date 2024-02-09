# Including a SubComponent

Subcomponents are a complex subject that rate a separate, independent tutorial devoted simply to them.  However, for completeness, this example will demonstrate a simple case of creating and including a subcomponent as part of a simulation.

As described in the SST SubComponent Migration Guide, "SubComponents are classes that are designed to be dynamically loadable into Components or other SubComponents."  This means subcomponents act as plugins.  You can create multiple different classes that provide the same service and expose the same interface, then specify which to use in the simulation configuration file.  This allows you to include different functionality without having to rebuild the simulation.

A good example of this is a random number generator.  The algorithm most suitable to generate a random number can vary depending upon the application.  By creating a different subcomponent for each, the algorithm used in a simulation can be specified at runtime and changed as necessary without recompiling the simulation.

For this example, the simulation will be expanded to include a simple subcomponent that supplies the value included as the message payload.  Effectively, the only change is to move functionality that exists in the clock event handler to the subcomponent but this serves to demonstrate how to create a subcomponent and use it in a simulation.

For this example we'll need to make the following changes.

1.  Create the subcomponent.
2.  Update the component to include the subcomponent.
3.  Modify the clock event handler to use the subcomponent.

## Example Setup

To set up for this example, perform the following steps:

1.  Navigate to the `example` folder and create a folder named `Example04`.
2.  Copy the `Example03` files to the `Example04` folder.
3.  Open the copied `ExampleComponent.h` file and change the namespace to `Example04`.
4.  Open the copied `ExampleComponent.cc` file and change the `using namespace` directive to refer to the `Example04` namespace.
5.  Open the copied `ExampleEvent.h` file and change the namespace to `Example04`.

## Creating a SubComponent

Creating a subcomponent is similar to creating a regular component.  First, navigate to the Example04 folder and, using the editor of your choice, create an new file and enter the code shown below.  Save it as `ExampleSubComponent.h`.

```
#ifndef EXAMPLE_SUBCOMPONENT_H
#define EXAMPLE_SUBCOMPONENT_H

#include <sst/core/subcomponent.h>
#include <sst/core/output.h>

const uint64_t FATAL = 0;
const uint64_t WARN  = 1;
const uint64_t INFO  = 2;
const uint64_t DEBUG = 3;
const uint64_t TRACE = 4;
const uint64_t ALL   = 5;

using namespace SST;

namespace Example04 
{
    class ExampleSubComponent : public SST::SubComponent
    {
        public:
            // Legacy constructors included for background compatibility.
            // Needed for V9, can be removed for V10.
            //
            ExampleSubComponent(Component *owningComponent) :
                SubComponent(owningComponent)
            { }
            ExampleSubComponent(Component* comp, Params& params) :
                SubComponent(comp)
            { }

            // Updated constructors.  No longer pass in a parent.
            // Instead, use a component id.
            //
            ExampleSubComponent(ComponentId_t id) :
                SubComponent(id)
            { }
            ExampleSubComponent(ComponentId_t id, Params& params) :
                SubComponent(id)
            { }

            virtual ~ExampleSubComponent() {}
            
            // Clock handler.  This is the method called from the clock event.
            //
            virtual void clock(Cycle_t) {}

            // Subcomponent public method for returning current count.
            //
            int getCount() { return count_++; }

            // Register the subcomponent API.
            //
            SST_ELI_REGISTER_SUBCOMPONENT_API(Example04::ExampleSubComponent)

            // Register the subcomponent.
            //
            SST_ELI_REGISTER_SUBCOMPONENT(
                ExampleSubComponent,                // INSERT_CLASS_NAME
                "example",                          // Element library.  Name of the .so.
                "ExampleSubComponent",              // "INSERT_CLASS_NAME"
                SST_ELI_ELEMENT_VERSION(1, 0, 0),   // 
                "Example subcomponent",             // Subcomponent description
                "SST::SubComponent"                 // "INSERT_FULL_PARENT_CLASS_NAME" or 
                                                    // "INSERT_COMPLETE_NAMESPACE::INSERT_PARENT_CLASS_NAME"
            )

        private:
            int count_ = 0;
    };
}

#endif
```
Like a component definition, the first few lines

```
#include <sst/core/subcomponent.h>
#include <sst/core/output.h>
```

specify include files needed from the SST core libraries.  The subcomponent.h file defines the SST base subcomponent from which all SST subcomponents are derived.  The output.h file contains definitions used for logging.

Next the namespace is defined

```
namespace Example04
{
```

and the subcomponent defined.

```
    class ExampleSubComponent : public SST::SubComponent
    {
        public:
            // Legacy constructors included for background compatibility.
            // Needed for V9, can be removed for V10.
            //
            ExampleSubComponent(Component *owningComponent) :
                SubComponent(owningComponent)
            { }
            ExampleSubComponent(Component* comp, Params& params) :
                SubComponent(comp)
            { }

            // Updated constructors.  No longer pass in a parent.
            // Instead, use a component id.
            //
            ExampleSubComponent(ComponentId_t id) :
                SubComponent(id)
            { }
            ExampleSubComponent(ComponentId_t id, Params& params) :
                SubComponent(id)
            { }

            virtual ~ExampleSubComponent() {}
            
            // Clock handler.  This is the method called from the clock event.
            //
            virtual void clock(Cycle_t) {}

            // Subcomponent public method for returning current count.
            //
            int getCount() { return count_++; }
```

As is the case for components and events, all subcomponents are derived from SST::SubComponent or one of its children.

The constructor and destructor are a standard format and required to interface with the SST core functions.  

Subcomponents still have clock handlers but they are not automatically called by the simulation.  Instead they are called from the subcomponent parent as part of the parent's clock event handling.

Finally, `getCount()` is the user method that provides the subcomponent functionality.  It simply increments and returns the count.

The subcomponent is described to the SST core using SST_ELI macros.

```
            // Register the subcomponent API.
            //
            SST_ELI_REGISTER_SUBCOMPONENT_API(Example04::ExampleSubComponent)

            // Register the subcomponent.
            //
            SST_ELI_REGISTER_SUBCOMPONENT_DERIVED(
                ExampleSubComponent,                // Name of the class being registered.
                                                    // INSERT_CLASS_NAME
                "example",                          // Element library.  Name of the .so.
                "ExampleSubComponent",              // Name used to reference the subcomponent.
                                                    // Can be anything but typically set to
                                                    // the class name.
                SST_ELI_ELEMENT_VERSION(1, 0, 0),   // 
                "Example subcomponent",             // Brief subcomponent description
                Example04::ExampleSubComponent      // Name of the subcomponent interface the
                                                    // subcomponent inherits from.  Should be
                                                    // the full parent class name.
                                                    // "INSERT_FULL_PARENT_CLASS_NAME" or 
                                                    // "INSERT_COMPLETE_NAMESPACE::INSERT_PARENT_CLASS_NAME"
            )
```
`SST_ELI_REGISTER_SUBCOMPONENT` registers the subcomponent's interface with SST.  This allows the subcomponent to serve as a parent for other subcomponents, including itself.

`SST_ELI_REGISTER_SUBCOMPONENT_DERIVED` registers the component with the ELI database, making it available for use by the simulation configuration file.  Its parameters are similar to those used when registering a component.

There are other ELI macros that may be necessary depending upon circumstances.  For example, if statistics are defined for the subcomponent they must be documented using the `SST_ELI_DOCUMENT_STATISTICS` macro.  However, for this simple case, the two reference above will be sufficient.

Finally, a single state variable is defined for use by the subcomponent.

```
        private:
            int count_ = 0;
```

## Update the Component

Open the file `ExampleComponent.h` and add a reference to the subcomponent and ExampleSubComponent include files.

```
#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/link.h>
#include <sst/core/output.h>
#include <sst/core/params.h>

#include "ExampleSubComponent.h"
```

Then add a public variable to hold the subcomponents.  Similar to links, subcomponents are stored in subcomponent slots.  The difference is, each slot can hold multiple subcomponents so a vector of subcomponents is created rather than a single subcomponent.   

```
            // Links to connect to other component.
            // The link connects to the components port.
            //
            SST::Link* link_;

            // Slot to hold a subcomponent.
            //
            std::vector<ExampleSubComponent*> subcomponentSlots_;
```

Subcomponent slots are registered with SST via the `ELI SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS` macro, so insert the following code below the `SST_ELI_DOCUMENT_PORTS` macro.

```
            // Document the subcomponent slots.
            //
            SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
                // Slot name, description, slot type
                //
                // Slot name is just a name.  It can be anything that makes sense but will
                // be used later to refer to this slot.
                //
                // Description is just that.  Can be anything.
                //
                // Slot type is the subcomponent fully qualified class name.
                //
                {"slot_", "Slot to hold a subcomponent", "Example04::ExampleSubComponent"}
            )
```

This registers a subcomponent named `"slot_"` with a description of `"Slot to hold a subcomponent"` and type `"Example04::ExampleSubComponent"` with SST so it can be used by the simulation.  

Save this file and open the file `ExampleComponent.cc`.  Insert the following code to configure the subcomponent into the constructor.

```
    // Configure the subcomponents.  The slot name is taken from where the 
    // subcomponent slot is documented in the component .h file.
    //
    SubComponentSlotInfo* info = getSubComponentSlotInfo("slot_");
    if ( !info ) 
    {
        // No defined subcomponents.  Print an error message and exit.
        //
        Output::getDefaultObject().fatal(CALL_INFO, -1, "Must specify at least one SubComponent for slot.\n");
    }
    else
    {
        // Create all the defined subcomponents.
        //
        long unsigned int maxSlot = info->getMaxPopulatedSlotNumber();
        logger_.verbose(CALL_INFO, DEBUG, 0x00, "There are %lu subcomponent slot entries\n", maxSlot + 1);
        for (int i = 0; i <= maxSlot; i++)
        {
            if (info->isPopulated(i))
            {
                logger_.verbose(CALL_INFO, DEBUG, 0x00, "Loading subcomponent into slot entry %d\n", i);        
                subcomponentSlots_.push_back(info->create<ExampleSubComponent>(i, ComponentInfo::SHARE_PORTS));
            }
        }
    }
```

This code assumes the subcomponent is defined in the configuration file and attempts to retrieve that configuration using the `getSubComponentSlotInfo()` method.  If a subcomponent has not been defined in the configuration file that is considered a fatal error.

If a subcomponent configuration is successfully retrieved from the configuration file, the subcomponents are created in the next code block.  There are two things to keep in mind when creating the subcomponents.  First, you can have more than one subcomponent of the same type assigned to a slot.  In this case each subcomponent is assigned a slot number in the configuration file.  Second, slot numbers do not have to be consecutive.  For example, you can have two subcomponents assigned slot numbers 1 and 5.

The code that creates the subcomponents takes this into account.  The maximum populated slot number is retrieved and subcomponents created only for those slot numbers associated with a subcomponent.

## Updating the Clock Event Handler

The updated clock event handler is listed below.

```
// Clock event handler.
//
bool ExampleComponent::clockTick(SST::Cycle_t cycle)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering clock for component id %lu\n", componentId_);
    bool done = false;

    // Call clockTick for each of the subcomponents.
    //
    logger_.verbose(CALL_INFO, INFO, 0x00, "Calling clock tick for subcomponents.\n");
    for ( auto subcomponent : subcomponentSlots_) 
    {
        subcomponent->clock(cycle);
    }

    // Send an event over the link.
    //
    uint64_t clockTickCount = subcomponentSlots_[0]->getCount();
    logger_.verbose(CALL_INFO, INFO, 0x00, "Sending event with payload %lu.\n", clockTickCount);
    SST::Event* ev = new ExampleEvent(clockTickCount);
    link_->send(ev);

    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}
```
It consists of two parts.  First, the clock event handler for each of the subcomponents is called.  

```
    // Call clockTick for each of the subcomponents.
    //
    logger_.verbose(CALL_INFO, INFO, 0x00, "Calling clock tick for subcomponents.\n");
    for ( auto subcomponent : subcomponentSlots_) 
    {
        subcomponent->clock(cycle);
    }
```
As mentioned previously, subcomponent clock event handlers are not called automatically so they must be handled by the subcomponent's parent within the parent's clock event handler.

Second, the clock count is retrieved from the subcomponent and sent over the link to the connected component.

```
    // Send an event over the link.
    //
    uint64_t clockTickCount = subcomponentSlots_[0]->getCount();
    logger_.verbose(CALL_INFO, INFO, 0x00, "Sending event with payload %lu.\n", clockTickCount);
    SST::Event* ev = new ExampleEvent(clockTickCount);
    link_->send(ev);
```
So what is the advantage of updating the clock tick count in a subcomponent over doing it as part of the component clock event handler?  If incrementing the clock tick count is all that is to be done there is no advantage.  However, consider the case where different ways to increment the clock tick count may be needed.  Perhaps in some cases the count will be decremented from a given value or a random value within a certain range will be returned.  In this case, multiple subcomponents each implementing a different way to update the clock tick count could be written.  Then, for each run of the simulation, it's only necessary to include a different subcomponent in the configuration file rather than recompiling the simulation.

## Updating the Configuration File

To include the subcomponent in the simulation, it's necessary to specify it in the configuration file when creating the component as shown below.

```
# Define the component.
#
# The parameters are a dictionary and can be any key/value pair defined
# by the component itself.
#
# The second parameter is <library>.<registered_name> specified in
# SST_ELI_REGISTER_COMPONENT.
#
component0 = sst.Component("component0", "example.ExampleComponent")
component0.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
    })
subcomponent0 = component0.setSubComponent("slot_", "example.ExampleSubComponent", 0)

component1 = sst.Component("component1", "example.ExampleComponent")
component1.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
})
subcomponent1 = component1.setSubComponent("slot_", "example.ExampleSubComponent", 0)

# Connect the objects to each other.
#
link = sst.Link("link0")
link.connect((component0, "link_", "5ns"), (component1, "link_",  "5ns"))
```

The `setSubComponent` method takes three parameters, the slot name as defined in the `SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS` macro, the fully qualified subcomponent class name in the form `<libraryname>.<subcomponentclass>`, and the previously described slot number.  One call to `setSubComponent` is required for each subcomponent to be used.

## Building and Executing the Simulation

The updated make file is listed below.

```
CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libexample.so install

libexample.so: ExampleComponent.cc\
		 	   ExampleComponent.h\
			   ExampleEvent.h\
			   ExampleSubComponent.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register example example_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libexample.so
```

It is identical to that of the previous example except for the addition of the file `ExampleSubComponent.h` in the list of source files.

Open a terminal, navigate to the Example04 folder, and execute the command

```
make clean; make
```

If there are errors, examine the source files to find the error and try again.

Once the build is complete, run the simulation using the command

```
sst ExampleConfig.py
```
For this example, the output below will be displayed.

```
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=34; Thread=0 -- Initializing component 0.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=37; Thread=0 -- Constructing new Example Instance.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=79; Thread=0 -- Successfully initialized clock.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=34; Thread=0 -- Initializing component 1.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=37; Thread=0 -- Constructing new Example Instance.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=79; Thread=0 -- Successfully initialized clock.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 0.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 0.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 1.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 1.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 2.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 2.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 3.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 3.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 4.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 4.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 5.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 5.
Time=6000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 0, compared to 5
Time=6000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 0, compared to 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 6.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 6.
Time=7000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 1, compared to 5
Time=7000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 1, compared to 5
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 7.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 7.
Time=8000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 2, compared to 5
Time=8000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 2, compared to 5
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 8.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 8.
Time=9000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 3, compared to 5
Time=9000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 3, compared to 5
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 9.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 9.
Time=10000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 4, compared to 5
Time=10000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 4, compared to 5
Time=11000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=11000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 10.
Time=11000; File=ExampleComponent.cc; Func=clockTick; Line=152; Thread=0 -- Calling clock tick for subcomponents.
Time=11000; File=ExampleComponent.cc; Func=clockTick; Line=161; Thread=0 -- Sending event with payload 10.
Time=11000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 5, compared to 5
Time=11000; File=ExampleComponent.cc; Func=messageHandler; Line=130; Thread=0 -- Ending sim for component 1.
Time=11000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 5, compared to 5
Time=11000; File=ExampleComponent.cc; Func=messageHandler; Line=130; Thread=0 -- Ending sim for component 0.
Simulation is complete, simulated time: 11 ns
```
