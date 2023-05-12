#ifndef _simpleCarWash_H
#define _simpleCarWash_H

#include <sst/core/component.h>
#include <sst/core/link.h>

#define WASH_BAY_EMPTY 0
#define WASH_BAY_FULL 1
#define NO_CAR 0
#define SMALL_CAR 1
#define LARGE_CAR 2

class simpleCarWash : public SST::Component {

public:
	simpleCarWash( SST::ComponentId_t id, SST::Params& params );
	~simpleCarWash();

	void setup();
	void finish();

	bool tick( SST::Cycle_t currentCycle );

	void handleEvent(SST::Event *ev);
	
	// Link list entry; used to store the cars as they are queued for service
	typedef struct CAR_RECORD_T {
		int CarSize; // 1 == Small Car; 2 == Large Car
		CAR_RECORD_T *ptrNext; // The next Car in the linked list	
	} CAR_RECORD;

	// Record to track how many cars have been washed
	typedef struct {
		int currentTime; // Time (ticks) since the epoch of the day
		int smallCarsWashed;
		int largeCarsWashed;
	} CARS_WASHED;
	
	// Record to track how many cars arrived in the past hour
	typedef struct {
		int smallCarsArrived;
		int largeCarsArrived;
		int noCarsArrived;
	} CAR_WASH_JOURNAL;

	// Record to manage the car wash bay itself
	typedef struct {
		int Occupied;
		int timeToOccupy;
		int WashBaySize;
	} CAR_WASH_BAY;
	
	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		simpleCarWash, // class
		"simpleCarWash", // element library
		"carWash", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"simpleCarWash main simulator. Accepts car inputs from a carGenerator component, then queues and washes cars using 1 small bay and 1 large bay",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "simulationTime", "How long to run simulation (hours)", "24" },
		{ "tickFreq", "How long 1 clock tick represents", "60s" },
		{ "smallCarWashTime", "How long to wash a small car (minutes)", "3" },
		{ "largeCarWashTime", "How long to wash a large car (minutes)", "5" }
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "port", "Port on which to receive cars", {"sst.Interfaces.StringEvent"}}
	)

private:
	SST::Output output;
	CAR_RECORD *ptrCarRecordList;
	CARS_WASHED CarWash;
	CAR_WASH_JOURNAL HourlyReport;
	CAR_WASH_BAY LargeBay;
	CAR_WASH_BAY SmallBay;

	void QueueACar(int carSize);
	bool IsCarWashBayReady(CAR_WASH_BAY *ptrWashBay);
	void CarWashClockTick();
	
	int CarWashTick;
	std::string clock;
	int64_t runTime;
	float ticksPerHour;
	int CarType;
	int smallTime;
	int largeTime;
	SST::Link *port;
};

#endif
