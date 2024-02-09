// This is the component source file.
//
// Created for SST-Core Version (9.1.0)
//
// As of 24 June 2020 the documentation on the website is out of date.
// You can get an idea of what needs to be done but there was a major
// rewrite as of V7.  This is a version that will run on the most 
// recent version. 
//

#include "ExampleComponent.h"
#include "ExampleEvent.h"
#include <iostream>

using namespace Example03;

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
    // Note how the event handler is declared.
    //
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Configuring link.\n");
    link_ = configureLink("link_", 
        new SST::Event::Handler<ExampleComponent>(this, &ExampleComponent::messageHandler));
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Successfully configured the link.\n");

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

    // Send an event over the link.
    //
    logger_.verbose(CALL_INFO, INFO, 0x00, "Sending event with payload %lu.\n", clockTickCount_ + 1);
    clockTickCount_ += 1;
    SST::Event* ev = new ExampleEvent(clockTickCount_);
    link_->send(ev);

    logger_.verbose(CALL_INFO, TRACE, 0x00, "Leaving clock for component id %lu\n", componentId_);
    return done;
}


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
