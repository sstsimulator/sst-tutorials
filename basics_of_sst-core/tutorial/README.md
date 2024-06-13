# Introduction to Developing Custom Elements for the Structural Simulation Toolkit (SST)

As described in [SST for Newbies](https://sst-simulator.org/SSTPages/SSTDeveloperSSTForNewbies/):

> The [Structural Simulation Toolkit (SST)](https://github.com/sstsimulator) is an open-source software framework used for simulating large-scale High Performance Computing (HPC - i.e. Super Computer) systems.

SST is simultaneously more and less than this.  While it includes a significant number of libraries that allow for the simulation of high-performance computers, at its core it is a highly capable event driven simulator that can be used to simulate much more.

The examples given below are intended to serve as an introduction to creating custom components for use in SST, allowing it to serve as a generic event driven simulator.  They include:

+ __Example00 - The Simplest Simulation__ - A simulation consisting of only a single component that prints messages to the console
+ __Example01 - Communicating Between Two Components__ - How to set up a link between two components and pass messages between them
+ __Example02 - Sending a Payload__ - Expands on the Communicating Between Two Components example to include a payload in the message
+ __Example03 - Using a Message Handler__ - Shows how to use a message handler to process incoming message payloads
+ __Example04 - Including a SubComponent__ - Shows how to create and include a subcomponent to provide functionality to a parent component
+ __Example05 - Networking Components__ - Shows how a simulation structure can be generalized, with the configuration file used to define the final configuration.


These examples were originally developed for SST-core 9.0

It is hoped that, in addition to serving as an introduction SST component development, these examples will serve to increase understanding of the libraries included as part of SST and how they contribute to the simulation of high-performance computers.

