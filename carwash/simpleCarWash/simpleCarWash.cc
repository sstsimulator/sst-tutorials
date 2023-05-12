/**
Simple Car Wash simulation

There is a separate component which generates "cars" as an input
These cars are then queued up and run through the carwash
The time it took for each car to go through is recorded to keep track
of satisfied customers.
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include "simpleCarWash.h"

using SST::Interfaces::StringEvent;

simpleCarWash::simpleCarWash( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {

	output.init("simpleCarWash-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	runTime = params.find<int64_t>("simulationTime", 24);
	clock = params.find<std::string>("tickFreq", "60s");
	smallTime = params.find<int>("smallCarWashTime", 3);
	largeTime = params.find<int>("largeCarWashTime", 5);
	
	// Figure out the ticks per hour using the given tickFreq
	if ( clock.compare(clock.length()-1, 1, "s") == 0 ) {
		ticksPerHour = (60.0 / std::stof(clock)) * 60.0;
	}else if ( clock.compare(clock.length()-2, 2, "Hz") == 0 ) {
		ticksPerHour = std::stof(clock) * 60.0 * 60.0;
	}
	
	// tell the simulator not to end without us
	registerAsPrimaryComponent();
	primaryComponentDoNotEndSim();

	// Set our Main Clock (register main clocks)
	// This will automatically be called by the SST framework
	registerClock(clock, new SST::Clock::Handler<simpleCarWash>(this, &simpleCarWash::tick));
	
	// Configure our port
	port = configureLink("port", new SST::Event::Handler<simpleCarWash>(this, &simpleCarWash::handleEvent));
	if ( !port ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
	}
}


simpleCarWash::~simpleCarWash() {

}


// Initialize all of the simulation variables and parameters
void simpleCarWash::setup() {
	ptrCarRecordList = NULL;
	CarWash.currentTime = 0;
	CarWash.smallCarsWashed = 0;
	CarWash.largeCarsWashed =0;
	LargeBay.Occupied = WASH_BAY_EMPTY;
	LargeBay.timeToOccupy = 0;
	SmallBay.Occupied = WASH_BAY_EMPTY;
	SmallBay.timeToOccupy = 0;
	CarWashTick = 0;
	HourlyReport.smallCarsArrived = 0;
	HourlyReport.largeCarsArrived = 0;
	HourlyReport.noCarsArrived = 0;
	CarType = NO_CAR;
	
	std::cout << std::endl;
	std::cout << "Welcome to Uncle Ed's Fantastic, Awesome, and Incredible Carwash!" << std::endl;
	std::cout << "Simulation time will be: " << runTime << " hours" << std::endl;
	std::cout << std::endl;
}


// Print out statistics at the end
void simpleCarWash::finish() {
	CAR_RECORD *ptr = ptrCarRecordList;
	int SmallCarCustomers = 0;
	int LargeCarCustomers = 0;
	
	std::cout << std::endl;
	std::cout << "------------------------------------------------------------------" << std::endl;
	std::cout << "Uncle Ed's Carwash Simulation has completed!" << std::endl;
	std::cout << "Here's a summary of the results:" << std::endl;
	std::cout << "Simulation duration was: " << runTime << " hours of operation." << std::endl;
	std::cout << "Small Cars Washed: " << CarWash.smallCarsWashed << std::endl;
	std::cout << "Large Cars Washed: " << CarWash.largeCarsWashed << std::endl;
	std::cout << "Total Cars washed: " << (CarWash.smallCarsWashed + CarWash.largeCarsWashed) << std::endl;
	
	while(1) {
		if(NULL != ptr) {
			if(SMALL_CAR == ptr->CarSize)
				SmallCarCustomers++;
			else if(LARGE_CAR == ptr->CarSize)
				LargeCarCustomers++;
			ptr = ptr->ptrNext;
		}
		else {
			std::cout << std::endl;
			break;
		}
	}

	std::cout << "Customers that are still waiting in line" << std::endl;
	std::cout << "Small Cars: " << SmallCarCustomers << std::endl;
	std::cout << "Large Cars: " << LargeCarCustomers << std::endl;
	std::cout << "Total: " << SmallCarCustomers + LargeCarCustomers << std::endl;
	std::cout << "------------------------------------------------------------------" << std::endl;
}


// Receive events that contain the CarType, add the cars to the queue
void simpleCarWash::handleEvent(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		CarType = atoi(&(se->getString().c_str()[0]));
		if(NO_CAR == CarType) {
			HourlyReport.noCarsArrived++;
		}else{
			QueueACar(CarType);
			if(SMALL_CAR == CarType){
				HourlyReport.smallCarsArrived++;
			}else if(LARGE_CAR == CarType){
				HourlyReport.largeCarsArrived++;
			}
		}
	}
	delete ev;
}


// Runs every clock tick
bool simpleCarWash::tick( SST::Cycle_t currentCycle ) 
{
	CarWash.currentTime = CarWashTick;
	// Announce the top of the hour and give a little summary of where we are at with our carwashing business
	if((0 < CarWashTick) && (0 == (CarWashTick % static_cast<int>(ticksPerHour)))) {
		std::cout << "------------------------------------------------------------------" << std::endl;
		std::cout << "Time = " << CarWashTick / ticksPerHour << " another simulated hour has passed! Summary:" << std::endl;
		std::cout << "  Small Cars = " << HourlyReport.smallCarsArrived << std::endl; 
		std::cout << "  Large Cars = " << HourlyReport.largeCarsArrived << std::endl;
		std::cout << "  No Cars = " << HourlyReport.noCarsArrived << std::endl;
		std::cout << std::endl;
		HourlyReport.smallCarsArrived = 0;
		HourlyReport.largeCarsArrived = 0;
		HourlyReport.noCarsArrived = 0;
	}

	// Car Wash simulation has finished
	if((runTime * ticksPerHour) <= CarWash.currentTime) {
		primaryComponentOKToEndSim();
		return(true);
	}
	
	// See what carwash bays can be cycled, and if any new cars can be processed in
	CarWashClockTick();
	CarWashTick++;
	return(false);
}


// Add a car to the queue
void simpleCarWash::QueueACar(int carSize)
{
	// Find the end of the car queue
	CAR_RECORD *ptr = ptrCarRecordList;

	if(NULL != ptr)	{ // Check to see if the list is empty
		while(NULL != ptr->ptrNext){ // If not walk down the list to the end
			ptr = ptr->ptrNext;
		}
	}
	// Allocate a bit of memory, formatted for a car record, and set the pointers. 
	if(NULL == ptr) { // First entry is a special case
		if(NULL == (ptrCarRecordList = new CAR_RECORD)) {
			printf("Error allocating memory for the first new car wash record\n");
		}
		ptr = ptrCarRecordList; // Anchor the new list
	}else{
		if(NULL == (ptr->ptrNext = new CAR_RECORD)) {
			printf("Error allocating memory to create a new car wash record\n");
		}
		ptr = ptr->ptrNext;	// Index to that pointer
	}

	ptr->ptrNext = NULL; // Set the pointer
	ptr->CarSize = carSize; // Set the car's size
}


// Check status of the bay
bool simpleCarWash::IsCarWashBayReady(CAR_WASH_BAY *ptrWashBay)
{
	if(ptrWashBay->Occupied != WASH_BAY_EMPTY) {
		ptrWashBay->timeToOccupy--;
		if(ptrWashBay->timeToOccupy <= 0) {
			ptrWashBay->Occupied = WASH_BAY_EMPTY;
			return(true);
		}
	}

	return(false);
}


// On every clock tick this checks to see if cars can be put into an empty bay
void simpleCarWash::CarWashClockTick() 
{
	CAR_RECORD *pTemp;
	
	// Now let's see if we have a bay empty, or ready to be emptied
	// Return of true implies the bay is empty and ready for a new car
	IsCarWashBayReady(&LargeBay);
	IsCarWashBayReady(&SmallBay);

	// Now that we've check the bays, let's fill any that have emptied. 
	if(NULL != ptrCarRecordList) { // Are there any cars waiting? 
		// See if we can fill the small bay first
		if((SmallBay.Occupied == WASH_BAY_EMPTY) && (ptrCarRecordList->CarSize == SMALL_CAR)) {
			SmallBay.Occupied = WASH_BAY_FULL;
			SmallBay.timeToOccupy = smallTime;
			pTemp = ptrCarRecordList;
			ptrCarRecordList = ptrCarRecordList->ptrNext;
			CarWash.smallCarsWashed++;
			delete pTemp;
		}
	}

	// The simulation rules stipulate that a large bay can accomodate either sized cars, but we can adjust the wash time
	// Apparently we have somewhat intelligent car-washing equipment, of course this can change too. 
	if(NULL != ptrCarRecordList) { // Examine the linked list to see if there are any cars waiting to be washed?
		if(LargeBay.Occupied == WASH_BAY_EMPTY) { 
			LargeBay.Occupied = WASH_BAY_FULL;
			// Set the wash time based on the car's size
			if(ptrCarRecordList->CarSize == LARGE_CAR){
				LargeBay.timeToOccupy = largeTime;
				CarWash.largeCarsWashed++;
			}else{
				LargeBay.timeToOccupy = smallTime;
				CarWash.smallCarsWashed++;
			}
			pTemp = ptrCarRecordList;
			ptrCarRecordList = ptrCarRecordList->ptrNext;
			delete pTemp;
		}
	}

	// Since we might have moved a large car out of the way, let's recheck and see if we can place a small car
	if(NULL != ptrCarRecordList) { // Are there any cars waiting? 
		if((SmallBay.Occupied == WASH_BAY_EMPTY) && (ptrCarRecordList->CarSize == SMALL_CAR)) {
			SmallBay.Occupied = WASH_BAY_FULL;
			SmallBay.timeToOccupy = smallTime;
			pTemp = ptrCarRecordList;
			ptrCarRecordList = ptrCarRecordList->ptrNext;
			CarWash.smallCarsWashed++;
			delete pTemp;
		}
	}
}

