# ISPASS 2024 Tutorials

Presented by Patrick Lavin and Joseph Kenny

May 5, 2024

Indianapolis, IN

## Abstract

As computing systems become increasingly complex, simulation has taken on a pervasive role in the process of designing and analyzing proposed systems.
Often, simulation is the foremost method of understanding the intricacies of novel high-performance architectures, emerging technologies, and interconnect topologies; it is used for gathering crucial information about software performance, energy consumption, and network eﬀiciency.
In parallel with the increased use of simulations, there has been growth in the number of available models for specific applications, fomenting a growing urgency for interoperability, consistency, and communication between simulation tools and their developers.
To ease the use and interoperability of larger simulated systems, a standard communication methodology between models is needed.

We will introduce attendees to the Structural Simulation Toolkit, SST, as a framework for building interoperable simulators capable of modeling modern computer systems.

This tutorial will include an introduction to the core concepts in SST, after which we will look at the many architectural simulation components available in SST-Elements. We will then cover two in-depth examples: single-node, multi-core simulation using memHierarchy, and network simulation using Ember, Firefly, and Merlin.


## Development Environment

We have created a Docker container for this tutorial so that we don't have to spend time installing SST. There are several ways you can use this container, or, if you prefer, you can install SST on your own machine.

NOTE: You only need to follow one of the following sections to get set up for this tutorial. We suggest using Codespaces.

### Github Codespace (Strongly preferred)

Github Codespaces provides 180 core hours and 15GB of storage per month for free to all users. This tutorial should only use about 8 core hours, and the container is ~7GB. Other options are available if you have already used your free allocation.

If you need to free up space, or you would like to delete this container when you are done, you can navigate to https://github.com/codespaces/. It should be automatically deleted if you do not use it for 30 days.

The following instructions will create a Codespace running in a container with SST already installed. You will also have an editable copy of the repo.

1. Navigate to  https://github.com/sstsimulator/sst-tutorials/ (the top directory of this repo)
2. Click Code dropdown
3. Click ...
4. Click New with options...
5. Change Dev container configuration dropdown to ispass2024
6. Create codespace

Done! The setup may take 10 minutes or so as it downloads the container.

### VS Code Dev Containers

If you have developed with containers locally before, you may prefer this method. We are not experts so we cannot help debug this process, but we have tested it on an M1 Mac.

1. Download Docker Desktop
2. Sign in to a Docker Hub account
3. Download the `sstbuilder/ispass2024:latest` from Docker Hub.
4. Create a container from the image and run it
5. Download VS Code
6. Install the `Docker` and `Dev Containers` extensions
7. Clone this repo
8. Start a dev environment. It should automatically use the correct container.

### Run Docker or Podman

If you have a server with Docker or Podman installed, you can run the container there. The container does not have a text editor installed, so it is recommended that you open up a second terminal so that you can edit files.

```
git clone https://github.com/sstsimulator/sst-tutorials
podman pull sstbuilder/ispass2024
podman run -v $PWD/sst-tutorials:/home/jammy/sst-tutorials -ti --rm <image id>
```

### Install SST on your machine
You may install SST locally if you wish. Here are two options for doing so:

#### Option 1: Use a script
While not officially supported, there are a pair of scripts I use for setting up SST on new machines.

1. Download [env.sh](https://raw.githubusercontent.com/plavin/plavin.github.io/master/scripts/env.sh) and [init-sst.sh](https://raw.githubusercontent.com/plavin/plavin.github.io/master/scripts/init-sst.sh).
2. Delete the lines from `env.sh` that contain `PIN`.
3. `source env.sh`
4. `./init-sst.sh`
5. Download this repo (sst-tutorials).

#### Option 2: Use the official instructions
The official SST installation guide is [here](http://sst-simulator.org/SSTPages/SSTBuildAndInstall_13dot1dot0_SeriesDetailedBuildInstructions/).
You will also need to clone this repository.
