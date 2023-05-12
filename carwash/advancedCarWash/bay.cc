/**
Car Wash bay

This subcomponent is a bay that washes cars for the Car Wash
The bay can be a small bay or a large bay
Small bays can only wash small cars
Large bays can wash small or large cars
*/

#include <sst/core/sst_config.h>
#include "bay.h"

bay::bay(SST::ComponentId_t id, SST::Params &params) : carWashBay(id){
	
	output.init("bay-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

	// Get parameters
	bay_size = params.find<int>("size", 1);
	smallTime = params.find<int>("smallCarWashTime", 3);
	largeTime = params.find<int>("largeCarWashTime", 5);
	
	// Setup statistics
	carsWashed = registerStatistic<uint32_t>("carsWashed");
	idleTime = registerStatistic<uint32_t>("idleTime");
	
	occupied = WASH_BAY_EMPTY;
}

bay::~bay() {

}

// Enters a new car into the bay if empty
bool bay::newCar(int CarType){
	if(occupied == WASH_BAY_EMPTY){
		if(CarType == LARGE_CAR){
			if(bay_size == LARGE_CAR){
				timeToOccupy = largeTime;
				occupied = WASH_BAY_FULL;
				return true;
			}
		}else{
			timeToOccupy = smallTime;
			occupied = WASH_BAY_FULL;
			return true;
		}
	}
	return false;
}

// Return the bay size
int bay::baySize(){
	return bay_size;
}

// Return occupied status of the bay
int bay::isOccupied(){
	return occupied;
}

// Count down time on any cars in bay
void bay::tick() {
	if(occupied != WASH_BAY_EMPTY) {
		timeToOccupy--;
		if(timeToOccupy <= 0) {
			occupied = WASH_BAY_EMPTY;
			carsWashed->addData(1);
		}
	}else{
		idleTime->addData(1);
	}
}

