#ifndef EXAMPLE_COMPONENT_H
#define EXAMPLE_COMPONENT_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/output.h>
#include <sst/core/params.h>

namespace Example03
{
    const uint64_t FATAL = 0;
    const uint64_t WARN  = 1;
    const uint64_t INFO  = 2;
    const uint64_t DEBUG = 3;
    const uint64_t TRACE = 4;
    const uint64_t ALL   = 5;

    // This is a very simple component.  It has a single port and registers 
    // a clock.  It sends a message over the link each time the clock 
    // event handler is called.
    //
    // All components inherit from SST:Component
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

            // Clock handler.  This is the method called from the clock event.
            //
            bool clockTick(SST::Cycle_t cycle);

            // Shared documentation macros. 
            //
            SST_ELI_DOCUMENT_PARAMS(
                { "debug",      "Debug location:  (0: NONE, 1: STDOUT, 2: STDERR, 3: FILE)", "0" },
                { "clock",      "Component clock rate", "1GHz" },
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

        private:
            
            // Handler for incoming messages.
            //
            void messageHandler(SST::Event* ev);

            // Member variables for this example.
            //
            SST::Output logger_;
            uint64_t componentId_;
            uint64_t clockTicks_;
            uint64_t clockTickCount_;

    };  // Close the class
}   // Close the namespace

#endif
