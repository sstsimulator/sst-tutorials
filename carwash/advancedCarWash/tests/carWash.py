import sst

# Component carWash from element carWash (carWash.carWash), named "Main Car Wash"
carWashComponent = sst.Component("Main Car Wash", "carWash.carWash")
carWashComponent.addParams({
	"simulationTime" : "24", # simulate 24 hours
	"tickFreq" : "60s", # process cars every 60 seconds (1 tick = 1 minute)
	})

# Subcomponent slot "bay" using component bay from element carWash (carWash.bay), number 0
baySubcomponent0 = carWashComponent.setSubComponent("carWashBay", "carWash.bay", 0)
baySubcomponent0.addParams({
	"size" : "1", # small car bay
	"smallCarWashTime" : "3"
	})

# Subcomponent slot "bay" using component bay from element carWash (carWash.bay), number 1
baySubcomponent1 = carWashComponent.setSubComponent("carWashBay", "carWash.bay", 1)
baySubcomponent1.addParams({
	"size" : "2", # large car bay
	"smallCarWashTime" : "3",
	"largeCarWashTime" : "5"
	})

# Subcomponent slot "bay" using component bay from element carWash (carWash.bay), number 2
baySubcomponent2 = carWashComponent.setSubComponent("carWashBay", "carWash.bay", 2)
baySubcomponent2.addParams({
	"size" : "2", # large car bay
	"smallCarWashTime" : "3",
	"largeCarWashTime" : "5"
	})


# Component carGenerator from element carWash (carWash.carGenerator), named "Car Generator"
carGeneratorComponent = sst.Component("Car Generator", "carWash.carGenerator")
carGeneratorComponent.addParams({
	"delay" : "60s", # send a car every 60 seconds (1 tick = 1 minute)
	"randomseed" : "151515"
	})


# Enable all statistics up to level 1 for the Car Wash and bays
# In this example that means noCarEvents is NOT enabled
sst.setStatisticLoadLevel(1)
sst.enableAllStatisticsForAllComponents()
#carWashComponent.enableAllStatistics()
#baySubcomponent0.enableAllStatistics()
#baySubcomponent1.enableAllStatistics()
#baySubcomponent2.enableAllStatistics()

# Connect the carWash and carGenerator components
sst.Link("MyLink").connect( (carWashComponent, "port", "1ps"), (carGeneratorComponent, "port", "1ps") )
