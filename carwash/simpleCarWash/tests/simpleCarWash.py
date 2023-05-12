import sst

# Component carWash from element simpleCarWash (simpleCarWash.carWash), named "Main Car Wash"
carWashComponent = sst.Component("Main Car Wash", "simpleCarWash.carWash")
carWashComponent.addParams({
	"simulationTime" : "24", # simulate 24 hours
	"tickFreq" : "60s", # process cars every 60 seconds (1 tick = 1 minute)
	"smallCarWashTime" : "3", # 3 minutes to wash a small car
	"largeCarWashTime" : "5" # 5 minutes to wash a large car
	})

# Component carGenerator from element simpleCarWash (simpleCarWash.carGenerator), named "Car Generator"
carGeneratorComponent = sst.Component("Car Generator", "simpleCarWash.carGenerator")
carGeneratorComponent.addParams({
	"delay" : "60s", # send a car every 60 seconds (1 tick = 1 minute)
	"randomseed" : "151515"
	})

# Connect the carWash and carGenerator components
sst.Link("Road").connect( (carWashComponent, "port", "1ps"), (carGeneratorComponent, "port", "1ps") )
