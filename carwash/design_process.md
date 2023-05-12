# background business context 

Sue is the owner of "Ed's Fantastic, Awesome, and Incredible Carwash!" 
Ed built the carwash on a half acre lot in Middleville in 1973. 
Sue inherited her uncle's carwash in 2013. 

The carwash has seen ups and downs but overall has been moderately successful. 
Sue is pondering expansion of the carwash business, specifically an increase in the number of wash bays. 
Sue knows the current cost of operation, the charge per customer, how long a customer is willing to wait, 
the typical pattern of people waiting for a car wash, and the cost of building a new bay. 

Sue is looking for your help in understanding whether it would make financial sense to construct one or more new car wash bays. 

# is there an analytic solution available?

Before writing software, can a solution be calculated? If the answer is yes, then there is no need to implement code.

When there are multiple [stocastic variables](https://en.wikipedia.org/wiki/Random_variable) interacting, numerical simulation is a useful approach. 

# mapping the business scenario and questions to Discrete Event Simulation (DES)

If the model can be expressed in terms of timing and queues, then Discrete Event Simulation (DES) is relevant.

From the scenario, what should the model be? 

There are aspects that are not relevant: 
* the car wash was built in 1973
* the car wash ownership changed in 2013

Potential constraints:
* the car wash is on a half acre lot. Since wash bays use space, the number of wash bays is limited. We'll need to ask Sue what the upper bound would be.
* Sue has a limited budget
* the number of customers

A Discrete Event Simulation is focused on timing and queues, so the specific monetary cost values are not necessary for the model. 

TODO: what variables should be in the car wash model?

# which Discrete Event Simulation (DES) to use?

Selecting a Discrete Event Simulator is based on which programming language and what scale. 
* serial Python: SimPy
* parallel C++ (with MPI): Structural Simulation Toolkit (SST)

Writing your own DES is feasible. TODO: pros and cons. 

TODO: what other DES software candidates exist, and why use each?

# implementing the model using the Structural Simulation Toolkit 

SST is a [software framework](https://en.wikipedia.org/wiki/Software_framework), in that [inversion of control](https://en.wikipedia.org/wiki/Inversion_of_control) requires you to provide customized code that will be called by `sst`. There are two main aspects you need to provide SST: the set of components and the graph which is constructed from those components. Each component is a node in the graph, and the edges of the graph are called "links" in SST.
