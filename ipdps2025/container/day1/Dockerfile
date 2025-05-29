FROM ubuntu:24.04

LABEL org.opencontainers.image.authors="grvosku@sandia.gov"
LABEL org.opencontainers.image.source="github.com:sstsimulator/sst-tutorials/ipdps2025/container/day1"

RUN apt-get update \
    && apt-get -y install --no-install-recommends \
    automake \
    autoconf \
    make \
    g++ \
    libtool \
    libtool-bin \
    libncurses-dev \
    ca-certificates \
    python3-dev \
    libltdl-dev \
    graphviz \
    build-essential \
    && apt clean -y \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -rm -d /home/jammy -s /bin/bash -g root -u 1001 jammy -p "$(openssl passwd -1 jammy)"

ENV PATH="/home/jammy/sst/install/bin:${PATH}"

COPY sst /home/jammy/sst
COPY build-sst.sh /home/jammy/sst/build-sst.sh

RUN chown -R jammy /home/jammy
USER jammy

WORKDIR /home/jammy/sst
RUN ls
RUN ./build-sst.sh
WORKDIR /home/jammy
