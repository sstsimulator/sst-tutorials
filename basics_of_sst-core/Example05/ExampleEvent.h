#ifndef _EXAMPLE_EVENT_H
#define _EXAMPLE_EVENT_H

// Basic include files from SST
//
#include <sst/core/event.h>

namespace Example05
{
    // Class must inherit from SST::Event
    //
    class ExampleEvent : public SST::Event
    {
    public:
        // Constructor
        //
        ExampleEvent(const uint32_t payload_value) : 
            SST::Event(),
            payload_(payload_value) {}

        // Return the payload
        //
        uint32_t getPayload() const { return payload_; }

    private:

        // Private variables.
        //
        uint32_t payload_;

        // Default constructor is private.
        // This is required for serialization to work.
        //
        ExampleEvent() : 
            SST::Event() {}

        // Serialization code.  Serializes data before sending it out
        // over the link.
        //
        void serialize_order(SST::Core::Serialization::serializer &ser) override
        {
            Event::serialize_order(ser);
            ser & payload_; // Not sure what & means in this context but based
                            // upon other examples, it looks like you create a
                            // list of values to be serialized using the & 
                            // operator.  So the list would look like:
                            // ser & payload0;
                            // ser & payload1;
                            // etc.
        }
        ImplementSerializable(ExampleEvent);
    };
}

#endif
