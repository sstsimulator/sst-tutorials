
This SST model is comprised of two components, `simpleCarWash.carWash` and `simpleCarWash.carGenerator` that are connected by a "road" (link). 

The `carWash` has two "bays", each with a different bay size: large and small. The time it takes to wash a car in the two bay sizes is distinct and is set as a parameter.
The `carGenerator` creates cars that are either larger or small. 
Small cars can fit in either the small or large bay, while large cars can only fit in the large bay.

The `simpleCarWash` has a statically configured set of bays, 
whereas the `carWash` model in <https://github.com/lpsmodsim/examples/tree/master/carWash> has bays separated into a subcomponent and an arbitrary number of bays can be configured by the user in the Python Driver file.

