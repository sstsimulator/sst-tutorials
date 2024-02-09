# Execute from the command line with the command:
#   sst exampleConfig.py 2>&1 | tee test.log
#
import sst

# Initialize local variables.
#
clockTicks = "5"
clock = "1GHz"
debug = "2"     #0 = FATAL, 1 = WARN, 2 = INFO, 3 = DEBUG, 4 = TRACE, 5 = ALL

# Define the component.
#
# The parameters are a dictionary and can be any key/value pair defined
# by the component itself.
#
# The second parameter is <library>.<registered_name> specified in
# SST_ELI_REGISTER_COMPONENT.
#
component0 = sst.Component("component0", "example.ExampleComponent")
component0.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
    })
subcomponent0 = component0.setSubComponent("slot_", "example.ExampleSubComponent", 0)

component1 = sst.Component("component1", "example.ExampleComponent")
component1.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
})
subcomponent1 = component1.setSubComponent("slot_", "example.ExampleSubComponent", 0)

# Connect the objects to each other.
#
link = sst.Link("link0")
link.connect((component0, "link_", "5ns"), (component1, "link_",  "5ns"))
