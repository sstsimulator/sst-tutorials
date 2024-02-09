// Created for SST-Core Version (9.1.0)
//
#include "ExampleComponent.h"
#include <iostream>

using namespace Example02;

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

    // Create the logger.
    //
    logger_ = SST::Output("Time=@t; File=@f; Func=@p; Line=@l; Thread=@I -- ", Logger::LogLevel::Trace, 0x00, SST::Output::STDOUT);
    Logger::logTrace(logger_, CALL_INFO, "Entering constructor for component id %lu\n", componentId_);
    
    // Initialize the debug output instance.
    // Strings for debug output use the printf format.
    //
    Logger::logInfo(logger_, CALL_INFO, "Initializing component %lu.\n", id);
    Logger::logDebug(logger_, CALL_INFO, "Parameters successfully read from config file.\n");
    Logger::logDebug(logger_, CALL_INFO, "clockTicks = %lu\n", clockTicks_);
    Logger::logInfo(logger_, CALL_INFO, "Constructing new Example Instance.\n");

    // Configure the component clock.
    //
    Logger::logDebug(logger_, CALL_INFO, "Clock rate is: %s\n", clock.c_str());
    registerClock(clock,
        new SST::Clock::Handler<ExampleComponent>(this, &ExampleComponent::clockTick));
    Logger::logInfo(logger_, CALL_INFO, "Successfully initialized clock.\n");

    // Register this component with the simulation.
    //
    registerAsPrimaryComponent();
    Logger::logDebug(logger_, CALL_INFO, "Component registered as primary component.\n");
    primaryComponentDoNotEndSim();
    Logger::logDebug(logger_, CALL_INFO, "Simulation notified it should not end.\n");
    Logger::logTrace(logger_, CALL_INFO, "Leaving constructor for component id %lu\n", componentId_);
}


// Called after all components have been constructed and initialization
// has completed, but before simulation time has begin.
//
// This is where you should do any other initialization that needs done
// but could be accomplished in the constructure.
//
void ExampleComponent::setup(void)
{
    Logger::logTrace(logger_, CALL_INFO, "Entering setup for component id %lu\n", componentId_);
    Logger::logTrace(logger_, CALL_INFO, "Leaving setup for component id %lu\n", componentId_);
}


// Called after the simulation is complete but before the objects are
// destroyed.  This is a good place to print out statistics.
//
void ExampleComponent::finish(void)
{
    Logger::logTrace(logger_, CALL_INFO, "Entering finish for component id %lu\n", componentId_);
    Logger::logTrace(logger_, CALL_INFO, "Leaving finish for component id %lu\n", componentId_);
}


// Clock event handler.
//
bool ExampleComponent::clockTick(SST::Cycle_t cycle)
{
    Logger::logTrace(logger_, CALL_INFO, "Entering clock for component id %lu\n", componentId_);

    // Increment the clock tick counter and end when you get to
    // the specified value.
    //
    clockTickCount_ += 1;
    bool done = (clockTickCount_ == clockTicks_);

    Logger::logInfo(logger_, CALL_INFO, "Clock tick count = %lu out of %lu\n", clockTickCount_, clockTicks_);
    if (done)
    {
        Logger::logInfo(logger_, CALL_INFO, "Ending sim.\n");
        primaryComponentOKToEndSim();
    }
   
    Logger::logTrace(logger_, CALL_INFO, "Leaving clock for component id %lu\n", componentId_);
    return done;
}