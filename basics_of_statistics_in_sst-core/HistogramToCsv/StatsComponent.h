#ifndef STATS_COMPONENT_H
#define STATS_COMPONENT_H

#include <sst/core/component.h>
#include <sst/core/output.h>
#include <sst/core/params.h>

namespace Stats
{
    // Define the log levels.  Each level displays its own message and the
    // levels above it.
    //
    // For example, ALL displays all log messages, INFO displays INFO, WARN,
    // and FATAL messages, FATAL displays only FATAL messages.
    //
    const uint64_t FATAL = 0;
    const uint64_t WARN  = 1;
    const uint64_t INFO  = 2;
    const uint64_t DEBUG = 3;
    const uint64_t TRACE = 4;
    const uint64_t ALL   = 5;

    // This is a very simple component.  It only registers a clock and 
    // adds data to two defined statistics.
    //
    class StatsComponent : public SST::Component
    {
        public:
            // Constructor/Destructor
            //
            StatsComponent(SST::ComponentId_t id, SST::Params &params);
            ~StatsComponent() {}

            // Standard SST::Component functions.  These all need to
            // be implemented in the component, even if they are empty.
            //
            void setup(void);
            void finish(void);

            // Clock handler.  This is the method called from the clock event.
            //
            bool clockTick(SST::Cycle_t cycle);

            // Shared documentation macros. 
            //
            SST_ELI_DOCUMENT_PARAMS(
                { "debug", "Debug level:  (0: NONE, 1: STDOUT, 2: STDERR, 3: FILE)", "0" },
                { "clock", "Component clock rate", "1GHz" }
            )
            
            SST_ELI_REGISTER_COMPONENT(
                StatsComponent,                         // Class name
                "statsexample",                         // Library name (the *.so)
                "StatsComponent",                       // Name used to reference the component.  This can be
                                                        // whatever you want it to be and will be referenced
                                                        // in the python configuration file.
                SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // Version number
                "Statistics example",                   // Description                 
                COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
            )

            // Document the statistics that this component provides
            // { "statistic_name", "description", "units", enable_level }
            //
            SST_ELI_DOCUMENT_STATISTICS( 
                {"INT64_f_clock_ticks", "Statistic to record the number of clock ticks, counting up", "ticks", 2},
                {"INT64_r_clock_ticks", "Statistic to record the number of clock ticks, counting down", "ticks", 2}
            )

        private:
            // Member variables for this example.
            //
            SST::Output logger_;                        // For displaying log messages.
            uint64_t componentId_;                      // SST supplied component id.
            uint64_t clockTicks_;                       // Counter to keep track of clock ticks.
            uint64_t maxClockTicks_;                    // Maximum number of clock ticks.
            Statistic<uint64_t>* forward_clock_ticks;   // Statistic to count forward clock ticks.
            Statistic<uint64_t>* reverse_clock_ticks;   // Statistic to count reverse clock ticks.

    };  // Close the class
}   // Close the namespace

#endif
