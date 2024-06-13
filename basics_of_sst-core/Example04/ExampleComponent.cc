// Created for SST-Core Version (9.1.0)
//
#include "ExampleComponent.h"
#include "ExampleEvent.h"
#include <iostream>

using namespace Example04;

// Component constructor.
//
ExampleComponent::ExampleComponent(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    componentId_(id),
    clockTickCount_(0)
{
    // Read in the parameters from the python config file.  See SST_ELI_DOCUMENT_PARAMS
    // for an explanation of what each parameter represents.
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
    logger_.verbose(CALL_INFO, INFO,  0x00, "Constructing new Example Instance.\n");

    // Configure the links (connections to other components).
    // The link is associated with a component's registered port.
    // No handler for this example.
    //
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Configuring link.\n");
    link_ = configureLink("link_", 
        new SST::Event::Handler<ExampleComponent>(this, &ExampleComponent::messageHandler));
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Successfully configured the link.\n");

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

    // Configure the component clock.
    //
    logger_.verbose(CALL_INFO, DEBUG, 0x00, "Clock rate is: %s\n", clock.c_str());
    registerClock(clock,
        new SST::Clock::Handler<ExampleComponent>(this, &ExampleComponent::clockTick));
    logger_.verbose(CALL_INFO, INFO,  0x00, "Successfully initialized clock.\n");

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
// This is where you should do any other initialization that needs done
// but could be accomplished in the constructure.
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

