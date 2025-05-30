FROM nvidia/cuda:11.7.1-devel-ubuntu22.04

SHELL ["/bin/bash", "-c"]

LABEL org.opencontainers.image.authors="grvosku@sandia.gov"
LABEL org.opencontainers.image.source="github.com:sstsimulator/sst-tutorials/ipdps2025/container/day2"

# Install CUDA
## GPGPU-Sim: cmake build-essential xutils-dev bison zlib1g-dev flex libglu1-mesa-dev git
## SST: automake autoconf make g++ libtool libtool-bin libncurses-dev python3-dev libltdl-dev graphviz
## riscv: autoconf automake autotools-dev curl python3 python3-pip python3-tomli libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev
RUN apt-get update \
    && apt-get install -y --no-install-recommends \ 
    cmake \
    build-essential \
    xutils-dev \
    bison \
    zlib1g-dev \
    flex \
    libglu1-mesa-dev \
    git \
    automake \
    autoconf \
    make \
    g++ \
    libtool \
    libtool-bin \
    libncurses-dev \
    python3-dev \
    libltdl-dev \
    graphviz \
    gawk \
    autotools-dev \
    curl \
    python3 \
    python3-pip \
    python3-tomli \
    libmpc-dev libmpfr-dev libgmp-dev \
    texinfo gperf \
    patchutils bc zlib1g-dev libexpat-dev ninja-build libglib2.0-dev libslirp-dev \
    && apt-get clean

RUN useradd -rm -d /home/jammy -s /bin/bash -g root -u 1001 jammy -p "$(openssl passwd -1 jammy)"

ENV CUDA_INSTALL_PATH /usr/local/cuda
ENV PTXAS_CUDA_INSTALL_PATH /usr/local/cuda
ENV PATH="/home/jammy/sst/install/bin:${PATH}"
ENV RISCV /opt/riscv
ENV RISCV_TOOLCHAIN_INSTALL_PATH /opt/riscv
ENV GPGPUSIM_ROOT /home/jammy/sst/gpgpu-sim_distribution

COPY sst /home/jammy/sst
COPY build-sst.sh /home/jammy/sst/build-sst.sh

# Build RISCV toolchain
WORKDIR /home/jammy/sst/riscv-gnu-toolchain
RUN ./configure --prefix=/opt/riscv --enable-multilib
RUN make -j

WORKDIR /home/jammy/sst
RUN rm -rf riscv-gnu-toolchain

# Build SST, Rev, gpgpu-sim as user
RUN chown -R jammy /home/jammy
USER jammy

# Source setup_environment
WORKDIR /home/jammy/sst/gpgpu-sim_distribution
RUN . ./setup_environment sst

WORKDIR /home/jammy/sst
RUN ls
RUN ./build-sst.sh
WORKDIR /home/jammy

# TODO
# Build or copy GPU examples in

# Delete rv toolchain to save space
USER root
RUN rm -rf /opt/riscv

USER jammy
WORKDIR /home/jammy

