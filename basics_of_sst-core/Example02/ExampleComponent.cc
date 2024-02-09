// Created for SST-Core Version (9.1.0)
//

#include "ExampleComponent.h"
#include "ExampleEvent.h"
#include <iostream>

using namespace Example02;

// Component constructor.
//
ExampleComponent::ExampleComponent(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    componentId_(id),
    clockTicks_(0),
    clockTickCount_(0)
{
    // Read in the parameters from the python config file.
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
// This is where you should do any other initialization that needs to be done
// but could be accomplished in the constructure or init().
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