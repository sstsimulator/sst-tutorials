# Sending a Payload

In the previous example empty messages were passed between two components.  This is suitable to demonstrate how to send messages but in any practical simulation these messages will include a payload.

To demonstrate how to add a payload to a message, the simulation will be modified so that instead of counting clock ticks locally, the count will be sent to the other component as the message payload.  The simulation will be ended when a message with the specified clock count is received.

For this example the following changes are required.

1.  Update the event to include a message payload.
2.  Update the clock event handler to process the incoming message payload.

## Example Setup

To set up for this example, perform the following steps:

1.  Navigate to the `example` folder created in `Example01` and create a folder named `Example02`.
2.  Copy the `Example01` files to the `Example02` folder.
3.  Open the copied `ExampleComponent.h` file and change the namespace to `Example02`.
4.  Open the copied `ExampleComponent.cc` file and change the `using namespace` directive to refer to the `Example02` namespace.
5.  Open the copied `ExampleEvent.h` file and change the namespace to `Example02`.

## Updating the Event

To update the event to include a payload as part of the message, open the file `ExampleEvent.h` in the editor of your choice.

First, add a private variable to hold the payload.

```
    private:

        // Private variables.
        //
        uint32_t payload_;
```

Update the constructor to accept the payload value as a parameter.  Also, update the getPayload() method to return that value as shown below.

```
    public:
        // Constructor
        //
        ExampleEvent(const uint32_t payload_value) : 
            SST::Event(),
            payload_(payload_value) {}

        // Return the payload
        //
        uint32_t getPayload() const { return payload_; }
```

A default constructor is required for message serialization to work so one is added below the definition of the `payload_` variable.

```
        // Private variables.
        //
        uint32_t payload_;

        // Default constructor is private.
        // This is required for serialization to work.
        //
        ExampleEvent() : 
            SST::Event() {}
```

Finally update the serializer to include the payload variable.

```
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
```
The message is now ready to include a payload to be sent to the connected component.

## Updating the Clock Event Handler

To update the clock event handler, open the file `ExampleComponent.cc` in the editor of your choice and replace the `clockTick()` method with that shown below.

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
            // the specified value.
            //
            uint32_t payloadValue = ev->getPayload();
            done = (clockTicks_ == payloadValue);
            logger_.verbose(CALL_INFO, INFO, 0x00, "Payload value = %u, compared to %lu\n", payloadValue, clockTicks_);
            if (done)
            {
                logger_.verbose(CALL_INFO, INFO, 0x00, "Ending sim.\n");
                primaryComponentOKToEndSim();
            }

            // Don't forget to delete the event when you're done with it.
            //
            delete ev;
            ev = nullptr;        
        }
    }

    // Send an event over the link.
    //
    logger_.verbose(CALL_INFO, INFO, 0x00, "Sending event with payload %lu.\n", clockTickCount_ + 1);
    clockTickCount_ += 1;
    ExampleEvent* ev = new ExampleEvent(clockTickCount_);
    link_->send(ev);

    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}
```

The handler still polls to see if a message has been received but instead of incrementing a local variable to track the number of received messages, it extracts the message payload and compares it to the `clockTicks_` value.  

```
        // Increment the clock tick counter and end when you get to
        // the specified value.
        //
        uint32_t payloadValue = ev->getPayload();
        done = (clockTicks_ == payloadValue);
        logger_.verbose(CALL_INFO, INFO, 0x00, "Payload value = %u, compared to %lu\n", payloadValue, clockTicks_);
        if (done)
        {
            logger_.verbose(CALL_INFO, INFO, 0x00, "Ending sim.\n");
            primaryComponentOKToEndSim();
        }

        // Don't forget to delete the event when you're done with it.
        //
        delete ev;
        ev = nullptr;
```

If the received value matches the desired value, the component notifies the simulation that, as far as this component is concerned, it's ok to end the simulation.  Also, once an event is no longer needed it must be deleted to prevent a memory leak.  The reason for this is explained below.

Just like the previous example, the `clockTickCount_` variable is incremented and a message sent to the other component.  However, in this case the `clockTickCount_` value is included as the message payload.

```
    // Send an event over the link.
    //
    logger_.verbose(CALL_INFO, INFO, 0x00, "Sending event with payload %lu.\n", clockTickCount_ + 1);
    clockTickCount_ += 1;
    ev = new ExampleEvent(clockTickCount_);
    link_->send(ev);
```

You may notice that, in this example, a new event is created each time a message is sent.  This is because each message contains its own payload.  Because a new event is created for each message, it must be deleted when it is no longer needed to prevent a memory leak, as mentioned above. 

## Building and Executing the Simulation

No changes to the simulation configuration or make file are required.  To run the simulation, navigate to the Example02 folder, and execute the command

```
make clean; make
```

to build it.  If there are errors, examine the source files to locate the error and try again.

Once the build is complete, run the simulation using the command

```
sst ExampleConfig.py
```
For this example, the output below will be displayed.

```
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=35; Thread=0 -- Initializing component 0.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=48; Thread=0 -- Clock rate is: 1GHz
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=35; Thread=0 -- Initializing component 1.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=48; Thread=0 -- Clock rate is: 1GHz
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 1.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 1.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 2.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 2.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 3.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 3.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 4.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 4.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 5.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 5.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 1, compared to 5
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 6.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 1, compared to 5
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 6.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 2, compared to 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 7.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 2, compared to 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 7.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 3, compared to 5
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 8.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 3, compared to 5
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 8.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 4, compared to 5
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 9.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 4, compared to 5
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 9.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 5, compared to 5
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=115; Thread=0 -- Ending sim.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 10.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=112; Thread=0 -- Payload value = 5, compared to 5
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=115; Thread=0 -- Ending sim.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=128; Thread=0 -- Sending event with payload 10.
Simulation is complete, simulated time: 10 ns
```

## Summary

This example expands on how to send a message by showing how to include a payload.  However, the messages are still being processed within the clock event handler.  This works for simple simulations but as simulations become more complex the amount of code within the clock event handler becomes unmanageable.

The next example will demonstrate how to address this through the use of message handlers.
