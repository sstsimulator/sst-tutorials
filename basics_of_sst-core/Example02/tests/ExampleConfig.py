# Execute from the command line with the command:
#   sst ExampleConfig.py 2>&1 | tee test.log
#
import sst

# Initialize local variables.
#
clockTicks = "5"
clock = "1GHz"
debug = "2"     #0 = FATAL, 1 = WARN, 2 = INFO, 3 = DEBUG, 4 = TRACE, 5 = ALL

componentName0 = "example_00"
componentName1 = "example_01"

# Define the component.
#
# The parameters are a dictionary and can be any key/value pair defined
# by the component itself.
#
# The second parameter is <library>.<registered_name>
#
obj0 = sst.Component(componentName0, "example.ExampleComponent")
obj0.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
    })

obj1 = sst.Component(componentName1, "example.ExampleComponent")
obj1.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    "debug"      : debug
})

# Connect the objects to each other.
#
link = sst.Link("link0")
link.connect((obj0, "link_", "5ns"), (obj1, "link_",  "5ns"))
