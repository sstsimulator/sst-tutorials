# IPDPS 2025 Tutorials

June 3 and 4, 2025

Milan, Italy

* [Tutorial Webpage](https://sst-simulator.org/SSTPages/SSTTutorial_IPDPS2025/)

## Abstract

As the science of high-performance computing (HPC) evolves, there is a growing need to understand and quantify the performance and compositional value of emerging technologies. Modeling and simulation techniques are well positioned to serve this purpose. The Structural Simulation Toolkit (SST) is a parallel discrete event-driven simulation framework that provides tools to enable co-design of HPC systems – from application to architecture. Tutorial participants will be introduced to key facets of conducting reproducible simulations of HPC architectures and infrastructures. This tutorial will be split into two self-contained sections: one focused on system-level modeling with an emphasis on application performance analysis and one focused on node-level modeling with an emphasis on architectural research.

*If you are new to SST, we recommend attending at least the “introduction” portion of the first tutorial prior to attending the second.*

Both tutorial session agendas are posted on the [tutorial webpage](https://sst-simulator.org/SSTPages/SSTTutorial_IPDPS2025/).

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
podman pull sstbuilder/ipdps2025
podman run -v $PWD/sst-tutorials:/home/jammy/sst-tutorials -ti --rm <image id>
```

### Install SST on your machine
You may install SST locally if you wish. Here are two options for doing so:

#### Option 1: Use a script
While not officially supported, there are a pair of scripts that may be useful for setting up SST on new machines.

1. Download [env.sh](https://raw.githubusercontent.com/plavin/plavin.github.io/master/scripts/env.sh) and [init-sst.sh](https://raw.githubusercontent.com/plavin/plavin.github.io/master/scripts/init-sst.sh).
2. Delete the lines from `env.sh` that contain `PIN`.
3. `source env.sh`
4. `./init-sst.sh`
5. Download this repo (sst-tutorials).

#### Option 2: Use the official instructions
You will need to [download the SST 15.0 release tarballs](https://sst-simulator.org/SSTPages/SSTMainDownloads) for sst-core and sst-elements as well as clone this repository.
The official SST installation guide is [here](https://sst-simulator.org/SSTPages/SSTBuildAndInstall_15dot0dot0_SeriesDetailedBuildInstructions/).
There are also quick-start instructions [on this page](https://sst-simulator.org/sst-docs/docs/guides/start) if you already have the SST dependencies available. 
