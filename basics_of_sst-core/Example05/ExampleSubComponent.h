#ifndef EXAMPLE_SUBCOMPONENT_H
#define EXAMPLE_SUBCOMPONENT_H

#include <sst/core/subcomponent.h>
#include <sst/core/output.h>

#include "ExampleEvent.h"

const uint64_t FATAL = 0;
const uint64_t WARN  = 1;
const uint64_t INFO  = 2;
const uint64_t DEBUG = 3;
const uint64_t TRACE = 4;
const uint64_t ALL   = 5;

using namespace SST;

namespace Example05 
{
    class ExampleSubComponent : public SST::SubComponent
    {
        public:

            // Updated constructors.  No longer pass in a parent.
            // Instead, use a component id.
            //
            ExampleSubComponent(ComponentId_t id) :
                SubComponent(id)
            { 
            }

            ExampleSubComponent(ComponentId_t id, Params& params) :
                SubComponent(id)
            {        
            }

            virtual ~ExampleSubComponent() {}


            // Method to initialize the subcomponent.  Serves the same purpose as the 
            // start() method in a component.
            //
            void start(SST::ComponentId_t id, SST::Params &params, 
                uint64_t slotNumber)
            {
                // Process the incoming paramaters.
                //
                componentId_ = id;
                slotNumber_ = slotNumber;

                std::string clock = 
                    params.find<std::string>("clock", "1GHz");  // Simulation clock rate.  Default to 1 GHz.
                unsigned int debug = 
                    params.find<int>("debug", ALL);             // Debug level.  Default to ALL.
                
                // Perform object initialization.
                //
                logger_ = SST::Output("Time=@t; File=@f; Func=@p; Line=@l; Thread=@I -- ", debug, 0x01, SST::Output::STDOUT);
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Initializing subcomponent, slot number %lu in component id %lu\n", 
                    slotNumber_, componentId_);

                link_ = configureLink("link_", clock,
                    new SST::Event::Handler<ExampleSubComponent>(this, &ExampleSubComponent::messageHandler)); 
                
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Subcomponent initialized.\n");
            }

            // Send the count over the link.
            //
            void sendCount() 
            {
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering subcomponent method sendCount()\n");
                logger_.verbose(CALL_INFO, DEBUG, 0x00, "Creating message\n");
                ExampleEvent* ev = new ExampleEvent(count_++);

                logger_.verbose(CALL_INFO, INFO, 0x00, "Sending message from component %lu, slot %lu with count %u\n", 
                    componentId_, slotNumber_, ev->getPayload() );
                link_->send(ev);

                logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving subcomponent method sendCount()\n");
            }

            // Return the current clock tick count.
            //
            int getCount()
            {
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering subcomponent method getCount()\n");
                return clockTickCount_;
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving subcomponent method getCount()\n");
            }

            // Clock handler.  This is the method called from the parent clock event.
            //
            virtual void clock(Cycle_t) 
            {
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering subcomponent method clock\n");
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving subcomponent method clock\n");
            }

            // Event handler for incoming messages.
            //
            void messageHandler(SST::Event* event)
            {
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering subcomponent messageHandler()\n");

                // Save the incoming value.
                //
                ExampleEvent* ev = static_cast<ExampleEvent*>(event);
                clockTickCount_ = ev->getPayload();
                logger_.verbose(CALL_INFO, INFO, 0x00, "Received clock tick %lu on component %lu at slot number %lu\n", 
                    clockTickCount_, componentId_, slotNumber_);

                // Don't forget to delete the event when you're done with it.
                // Otherwise you'll get a serious memory leak.
                //
                delete ev;
                logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving subcomponent messageHandler()\n");
            }

            // Document the component ports.
            //
            // Port name, description, vector fo supported events.
            //
            // Port name is just a name.  It can be anything that makes sense but will
            // be used later to refer to this port.
            //
            // Description is just that.  Can be anything.
            //
            // Support events is a std::vector of the names of supported events.  These
            // are initialized as {"lib1.event1", "lib1.event2", "lib2.event3"}.
            // Note these are the names as strings, not types.
            //
            SST_ELI_DOCUMENT_PORTS(
                { "link_", "Message port", {}}
            )

            // Register the subcomponent API.
            //
            SST_ELI_REGISTER_SUBCOMPONENT_API(Example05::ExampleSubComponent)

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
                Example05::ExampleSubComponent      // Name of the subcomponent interface the
                                                    // subcomponent inherits from.  Should be
                                                    // the full parent class name.
                                                    // "INSERT_FULL_PARENT_CLASS_NAME" or 
                                                    // "INSERT_COMPLETE_NAMESPACE::INSERT_PARENT_CLASS_NAME"
            )

        private:
            // Links to connect to other component.
            // The link connects to the subcomponents port.
            //
            SST::Link* link_ = nullptr;

            // Count sent to the connected subcomponent.
            //
            SST::Output logger_;        // Used for logging messages.
            uint64_t componentId_;      // Id associated with the parent component.
            uint64_t slotNumber_;       // Slot number associated with the subcomponent.
            uint32_t count_ = 0;
            uint64_t clockTickCount_ = 0;
    };
}

#endif