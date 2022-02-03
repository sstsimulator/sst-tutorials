The code in this folder and the subfolders is compatible with SST-core release XX

Exercises for the SST Tutorial, "Building a Simulator"

# Ex1:    Simple, 4-component simulation:
- Miranda-based CPU, L1, L2, and Memory

# Ex2:    Same simulation, adds configurability
- Parsing command-line arguments, and reading parameters from a config file

# Ex3:    Multi-Core
- Merlin-based on-chip network
- Multiple cores on the chip
- Directory Controller in front of Memory Controller

# Ex4:    Modular CPUs
- Can swap-out Merlin for an Ariel-based simulator

# Ex 5:   Ring network
- Changed topology of on-chip network from a start topology to a Ring

# Ex 6:   Multiple memories
- Added additional memory controllers

# Ex 7:   Full Sandy Bridge
- Each ring-stop defines a group, split L3, an L2, L1 and core

# Ex 8:   Merlin topology generator
- Uses the built-in Merlin topology generator to build the ring

# Ex 9:   Multiple networks
- Uses the built-in Merlin topology generator to build a mesh network
- At each end-point on the mesh, defines a local network containing
    X cpus, Y L3 caches, Z memory controllers
