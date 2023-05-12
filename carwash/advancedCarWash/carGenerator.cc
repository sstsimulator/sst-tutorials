/**
Car Generator for carWash simulator

This component generates "cars" which are sent to the carwash
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include "carGenerator.h"

using SST::Interfaces::StringEvent;
using namespace SST::carWash;

carGenerator::carGenerator( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
	
	output.init("carGenerator-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	clock = params.find<std::string>("delay", "60s");
	RandomSeed = params.find<int64_t>("randomseed", 151515);

	// Register the clock
	registerClock(clock, new SST::Clock::Handler<carGenerator>(this, &carGenerator::clockTick));
	
	// Initialize random
	rng = new SST::RNG::MarsagliaRNG(11, RandomSeed);
	
	// Configure our port
	port = configureLink("port");
	if ( !port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
	}
}

carGenerator::~carGenerator() {

}

void carGenerator::setup() {
	output.verbose(CALL_INFO, 1, 0, "Component is being setup.\n");
}

void carGenerator::finish() {
	output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

bool carGenerator::clockTick( SST::Cycle_t currentCycle ) {
	// generating a random number between 0 and 2
	// 0 = No Car
	// 1 = Small Car
	// 2 = Large Car
	int rndNumber;
	rndNumber = (int)(rng->generateNextInt32());		
	rndNumber = (rndNumber & 0x0000FFFF) ^ ((rndNumber & 0xFFFF0000) >> 16);
	rndNumber = abs((int)(rndNumber % 3));

	port->send(new StringEvent(std::to_string(rndNumber)));
	return false;
}

