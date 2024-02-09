# Use baseimage-docker which is a modified Ubuntu specifically for Docker
# https://hub.docker.com/r/phusion/baseimage
# https://github.com/phusion/baseimage-docker
FROM phusion/baseimage:0.11

# Use baseimage-docker's init system
CMD ["/sbin/my_init"]

# Update and install packages
RUN apt update && apt -y upgrade && apt -y install \
    build-essential \
    doxygen \
    libtool-bin \
    graphviz \
    time \
    mpich \
    python3-dev \
    python3-pip \
    automake

# Clean up apt mess
RUN apt clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

ARG USER=sst
ARG UID=1000
ARG GID=1000
# default password for user
ARG PW=sst

RUN useradd -m ${USER} --uid=${UID} && echo "${USER}:${PW}" | chpasswd

USER ${UID}:${GID}
WORKDIR /home/${USER}

# Setup Environment for SST
ARG dir=/home/sst/build
RUN mkdir -p $dir
ARG SST_CORE_HOME=/home/sst/sst-core
ENV SST_CORE_HOME=/home/sst/sst-core
ENV PATH="$PATH:$SST_CORE_HOME/bin"

WORKDIR $dir

ARG SSTver=11.1.0

# from https://github.com/sstsimulator/sst-core/releases/tag/v11.0.0_Final
COPY sstcore-${SSTver}.tar.gz .
RUN tar zxvf sstcore-${SSTver}.tar.gz
RUN mv sstcore-${SSTver} sst-core

# Build SST Core
RUN cd $dir/sst-core && ./autogen.sh && \
  	./configure --prefix=$SST_CORE_HOME && \
	  make all install

WORKDIR /home/sst/

# Clean up SST junk
#RUN rm -rf $dir
