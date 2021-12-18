#ifndef MY_COMPONENT_H
#define MY_COMPONENT_H

#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/link.h>
#include <sst/core/output.h>
#include <sst/core/params.h>

#include "ExampleSubComponent.h"

namespace Example05
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

            // Links to connect to other component.
            // The link connects to the components port.
            //
            SST::Link* link_;

            // Slot to hold a subcomponent.
            //
            std::vector<std::pair<bool, ExampleSubComponent*>> subcomponentSlots_;

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
                "ExampleComponent",                     // Named use to reference the component.  This can be
                                                        // whatever you want it to be and will be referenced
                                                        // in the python configuration file.
                SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // Version number
                "Clock element example",                // Description                 
                COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
            )

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