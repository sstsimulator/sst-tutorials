# Execute from the command line with the command:
#   sst Example00Config.py 2>&1 | tee test.log
#
import sst

# Initialize local variables.
#
clockTicks = "10"   # Number of clock ticks before the simulation ends
clock = "1GHz"      # Simulation clock rate

componentName0 = "example00"

# Define the component.
#
# The parameters are a dictionary and can be any key/value pair defined
# by the component itself.
#
# The second parameter is <library>.<registered_name>
# These correspond to the second and third parameters of the
# SST_ELI_REGISTER_COMPONENT macro in Example00Component.h,
# respectively.
#
obj = sst.Component(componentName0, "example.ExampleComponent")
obj.addParams({
    "clock"      : clock,
    "clockTicks" : clockTicks,
    })
