# Using a Message Handler

Previous examples have demonstrated message processing using polling in the clock event handler.  This works for simple simulations with a limited number of links but as the simulations become more complex a better solution is to use message handlers.

A message handler is a method associated with a link that is called whenever a message is received over that link.  It is advantageous include eliminating unnecessary polling when no messages are available on a link and being able to separate processing of different message types.

To demonstrate how to use a message handler, the simulation will be modified so that instead of polling for messages in the clock event handler, a message handler will be attached to the link and the message processing performed within the handler.  As in previous examples, the clock tick count will be sent to the other component as a payload and the simulation will be ended when a message with the specified clock count is received.

For this example we'll need to make the following changes.

1.  Configure the link to use a message handler.
2.  Add the message handler to the component.
2.  Update the clock event handler to remove any incoming message processing.

## Example Setup

To set up for this example, perform the following steps:

1.  Navigate to the `example` folder created in `Example02` and create a folder named `Example03`.
2.  Copy the `Example02` files to the `Example03` folder.
3.  Open the copied `ExampleComponent.h` file and change the namespace to `Example03`.
4.  Open the copied `ExampleComponent.cc` file and change the `using namespace` directive to refer to the `Example03` namespace.
5.  Open the copied `ExampleEvent.h` file and change the namespace to `Example03`.

## Configuring the Link

Adding a message handler to the link configuration simply consists of adding an additional parameter to the event constructor used in `configureLink` call.  

Open the `ExampleComponent.cc` file and change the `configureLink` call to

```
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Configuring link.\n");
    link_ = configureLink("link_", 
        new SST::Event::Handler<ExampleComponent>(this, &ExampleComponent::messageHandler));
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Successfully configured the link.\n");
```
`messageHandler` is the name of the method used to process incoming messages, which will be defined next.

## Adding the Message Handler

To define the message handler, open the file `ExampleComponent.h` and add the message handler definition to the private class definitions.

```
        private:
            
            // Handler for incoming messages.
            //
            void messageHandler(SST::Event* ev);
```

Save the file, open the `ExampleComponent.cc` file, and add the message handler definition listed below to the bottom of the file.

```
// Event handler for incoming messages.
//
void ExampleComponent::messageHandler(SST::Event* event)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering message handler for component id %lu\n", componentId_);

    // Check the incoming value.  Compare it to the one specified on the
    // command line.
    //
    ExampleEvent* ev = static_cast<ExampleEvent*>(event);
    uint32_t payloadValue = ev->getPayload();
    bool done = (clockTicks_ == payloadValue);

    logger_.verbose(CALL_INFO, INFO, 0x00, "Payload value = %u, compared to %lu\n", payloadValue, clockTicks_);
    if (done)
    {
        logger_.verbose(CALL_INFO, INFO, 0x00, "Ending sim for component %lu.\n", componentId_);
        primaryComponentOKToEndSim();
    }

    // Don't forget to delete the event when you're done with it.
    // Otherwise you'll get a serious memory leak.
    //
    delete ev;

    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving message handler for component id %lu\n", componentId_);
}
```

The message event is passed in as a parameter.  Processing simply consists of getting the payload from the message and comparing it to the desired number of clock ticks.

```
    // Check the incoming value.  Compare it to the one specified on the
    // command line.
    //
    ExampleEvent* ev = static_cast<ExampleEvent*>(event);
    uint32_t payloadValue = ev->getPayload();
    bool done = (clockTicks_ == payloadValue);
```

Once the desired number of clock ticks is received, the method calls `primaryComponentOKToEndSim()` to notify the simulation that, as far as this component is concerned, it's ok to end the simulation.  Also, just as in the previous example, the event must be deleted when it is no longer needed to prevent a memory leak.

## Updating the Clock Event Handler

The updated clock event handler is listed below.

```
// Clock event handler.
//
bool ExampleComponent::clockTick(SST::Cycle_t cycle)
{
    logger_.verbose(CALL_INFO, TRACE, 0x00, "Entering clock for component id %lu\n", componentId_);
    bool done = false;

    // Send an event over the link.
    //
    logger_.verbose(CALL_INFO, INFO, 0x00, "Sending event with payload %lu.\n", clockTickCount_ + 1);
    clockTickCount_ += 1;
    SST::Event* ev = new ExampleEvent(clockTickCount_);
    link_->send(ev);

    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}
```
The differences consist of removing code previously used to check for incoming messages, which makes the handler shorter and easier to understand.

## Building and Executing the Simulation

No changes to the simulation configuration or make file are required.  To run the simulation, navigate to the Example03 folder, and execute the command

```
make clean; make
```

to build it.  If there are errors, examine the source files to find the error an try again.

Once the build is complete, run the simulation using the command

```
sst ExampleConfig.py
```
For this example, the output below will be displayed.

```
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=42; Thread=0 -- Initializing component 0.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=57; Thread=0 -- Clock rate is: 1GHz
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=42; Thread=0 -- Initializing component 1.
Time=0; File=ExampleComponent.cc; Func=ExampleComponent; Line=57; Thread=0 -- Clock rate is: 1GHz
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 1.
Time=1000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 1.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 2.
Time=2000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 2.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 3.
Time=3000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 3.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 4.
Time=4000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 4.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 5.
Time=5000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 5.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 6.
Time=6000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 6.
Time=6000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 1, compared to 5
Time=6000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 1, compared to 5
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 7.
Time=7000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 7.
Time=7000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 2, compared to 5
Time=7000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 2, compared to 5
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 8.
Time=8000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 8.
Time=8000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 3, compared to 5
Time=8000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 3, compared to 5
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 9.
Time=9000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 9.
Time=9000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 4, compared to 5
Time=9000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 4, compared to 5
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 10.
Time=10000; File=ExampleComponent.cc; Func=clockTick; Line=104; Thread=0 -- Sending event with payload 10.
Time=10000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 5, compared to 5
Time=10000; File=ExampleComponent.cc; Func=messageHandler; Line=130; Thread=0 -- Ending sim for component 1.
Time=10000; File=ExampleComponent.cc; Func=messageHandler; Line=127; Thread=0 -- Payload value = 5, compared to 5
Time=10000; File=ExampleComponent.cc; Func=messageHandler; Line=130; Thread=0 -- Ending sim for component 0.
Simulation is complete, simulated time: 10 ns
```
