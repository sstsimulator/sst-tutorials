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
subcomponent0_0 = component0.setSubComponent("slot_", "example.ExampleSubComponent", 0)
subcomponent0_1 = component0.setSubComponent("slot_", "example.ExampleSubComponent", 1)

component1 = sst.Component("component1", "example.ExampleComponent")
component1.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
})
subcomponent1_0 = component1.setSubComponent("slot_", "example.ExampleSubComponent", 0)
subcomponent1_1 = component1.setSubComponent("slot_", "example.ExampleSubComponent", 1)

component2 = sst.Component("component2", "example.ExampleComponent")
component2.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
})
subcomponent2_0 = component2.setSubComponent("slot_", "example.ExampleSubComponent", 0)
subcomponent2_1 = component2.setSubComponent("slot_", "example.ExampleSubComponent", 1)

# Connect the objects to each other.
#
link0_1 = sst.Link("link0_1")
link1_2 = sst.Link("link1_2")
link2_0 = sst.Link("link2_0")
link0_1.connect((subcomponent0_0, "link_", "5ns"), (subcomponent1_1, "link_", "5ns"))
link1_2.connect((subcomponent1_0, "link_", "5ns"), (subcomponent2_1, "link_", "5ns"))
link2_0.connect((subcomponent2_0, "link_", "5ns"), (subcomponent0_1, "link_", "5ns"))