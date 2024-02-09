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

        private:
            int count_ = 0;
    };
}

#endif