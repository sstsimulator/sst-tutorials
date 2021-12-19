#ifndef _carWash_H
#define _carWash_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/subcomponent.h>

#define WASH_BAY_EMPTY 0
#define WASH_BAY_FULL 1
#define NO_CAR 0
#define SMALL_CAR 1
#define LARGE_CAR 2

// carWash subcomponent prototype
class carWashBay : public SST::SubComponent {
public:
	// constructor cannot be virtual, all other functions must be
	carWashBay(SST::ComponentId_t id) : SubComponent(id) {};
	carWashBay(SST::ComponentId_t id, SST::Params& params) : SubComponent(id) {};
	virtual ~carWashBay() {};
	virtual bool newCar(int CarType) = 0;
	virtual int baySize() = 0;
	virtual int isOccupied() = 0;
	virtual void tick() {};
	SST_ELI_REGISTER_SUBCOMPONENT_API(carWashBay)
};


class carWash : public SST::Component {

public:
	carWash( SST::ComponentId_t id, SST::Params& params );
	~carWash();

	void setup();
	void finish();

	bool tick( SST::Cycle_t currentCycle );

	void handleEvent(SST::Event *ev);
	
	// Link list entry; used to store the cars as they are queued for service
	typedef struct CAR_RECORD_T {
		int CarSize; // 0 == Small Car; 1 == Large Car
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
	
	// Register the component
	SST_ELI_REGISTER_COMPONENT(
		carWash, // class
		"carWash", // element library
		"carWash", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
		"carWash main simulator. Accepts car inputs from a carGenerator component, then queues and washes cars using bay subcomponents",
		COMPONENT_CATEGORY_UNCATEGORIZED
	)

	// Parameter name, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{ "simulationTime", "How long to run simulation (hours)", "24" }
	)
	
	// Statistic name, description, unit, enable level
	SST_ELI_DOCUMENT_STATISTICS(
        { "smallCarsWashed", "Number of small cars washed", "cars", 1 },  
        { "largeCarsWashed", "Number of large cars washed", "cars", 1 },
        { "noCarEvents", "Number of no car generated events", "cars", 2 },
        { "smallCarsWaiting", "Number of small cars still waiting in line", "cars", 1 },
        { "largeCarsWaiting", "Number of large cars still waiting in line", "cars", 1 }
    )
	
	// Slot name, description, slot type
	SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
		{ "carWashBay", "Car Wash Bay", "SST::carWash::carWashBay" }
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{ "port", "Port on which to receive cars", {"sst.Interfaces.StringEvent"} }
	)

private:
	SST::Output output;
	CAR_RECORD *ptrCarRecordList;
	CARS_WASHED CarWash;
	CAR_WASH_JOURNAL HourlyReport;

	void QueueACar(int carSize);
	void CarWashClockTick();
	
	int CarWashTick;
	std::string clock;
	float ticksPerHour;
	int64_t runTime;
	int CarType;
	std::vector<carWashBay*> subComps;
	SST::Link *port;
	
	// Statistics
	Statistic<int>* smallCarsWashed;
	Statistic<int>* largeCarsWashed;
	Statistic<int>* noCarEvents;
	Statistic<int>* smallCarsWaiting;
	Statistic<int>* largeCarsWaiting;
};

#endif
