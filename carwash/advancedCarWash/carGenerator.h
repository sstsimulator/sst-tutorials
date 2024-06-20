#ifndef _carGenerator_H
#define _carGenerator_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>

namespace SST::carWash {

class carGenerator : public SST::Component {

public:
	carGenerator( SST::ComponentId_t id, SST::Params& params );
	~carGenerator();

	void setup();
	void finish();

	bool clockTick( SST::Cycle_t currentCycle );

	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		carGenerator, // class
		"carWash", // element library
		"carGenerator", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"Car Generator for the carwash",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "delay", "How long to wait between sending car events", "60s" },
		{ "randomseed", "Random Seed for car type generation", "151515" }
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "port", "Port on which cars are sent", {"sst.Interfaces.StringEvent"}}
	)

private:
	SST::Output output;
	SST::RNG::MarsagliaRNG* rng;
	std::string clock;
	int64_t RandomSeed;
	SST::Link *port;
};

}

#endif
