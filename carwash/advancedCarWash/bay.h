#ifndef _bay_H
#define _bay_H

#include "carWash.h"

class bay : public carWashBay {

public:
	bay( SST::ComponentId_t id, SST::Params& params );
	~bay();

	bool newCar(int CarType);
	int baySize();
	int isOccupied();
	void tick();

	// Register the subcomponent
	SST_ELI_REGISTER_SUBCOMPONENT_DERIVED(
		bay, // class
		"carWash", // element library
		"bay", // subcomponent
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"Bay subcomponent for the carwash",
		carWashBay // subcomponent slot
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "size", "Size of the bay (1 = small, 2 = large)", "1" },
		{ "smallCarWashTime", "How long to wash a small car (ticks)", "3" },
		{ "largeCarWashTime", "How long to wash a large car (ticks)", "5" }
	)
	
	// Statistic name, description, unit, enable level
	SST_ELI_DOCUMENT_STATISTICS(
        { "carsWashed", "Number of cars washed", "cars", 1 },
        { "idleTime", "Time spent not washing cars", "ticks", 1 }
    )


private:
	SST::Output output;
	int bay_size;
	int smallTime;
	int largeTime;
	int occupied;
	int timeToOccupy;
	Statistic<uint32_t>* carsWashed;
	Statistic<uint32_t>* idleTime;
};

#endif
