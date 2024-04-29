FROM ubuntu:22.04

LABEL org.opencontainers.image.authors="prlavin@sandia.gov"
LABEL org.opencontainers.image.source="github.com:sstsimulator/sst-tutorials/ispass2024/container"

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
    && apt clean -y \
    && rm -rf /var/lib/apt/lists/*

## removed git vim  cmake gcc openssh-client sudo

#RUN useradd -rm -d /home/jammy -s /bin/bash -g root -G sudo -u 1001 jammy -p "$(openssl passwd -1 jammy)"
RUN useradd -rm -d /home/jammy -s /bin/bash -g root -u 1001 jammy -p "$(openssl passwd -1 jammy)"

ENV PATH="/home/jammy/sst/install/bin:${PATH}"
#ENV INTEL_PIN_DIRECTORY=/home/jammy/sst/intel-pin
#ENV INTEL_PIN_DIR=${INTEL_PIN_DIRECTORY}
#ENV PIN_HOME=${INTEL_PIN_DIRECTORY}

COPY sst /home/jammy/sst
COPY build-sst.sh /home/jammy/sst/build-sst.sh
#COPY build-sst.sh /home/jammy/sst/
#COPY sst-core /home/jammy/sst/sst-core
#COPY sst-elements /home/jammy/sst/sst-elements
#COPY intel-pin /home/jammy/sst/intel-pin

RUN chown -R jammy /home/jammy
USER jammy

WORKDIR /home/jammy/sst
RUN ls
RUN ./build-sst.sh
WORKDIR /home/jammy
