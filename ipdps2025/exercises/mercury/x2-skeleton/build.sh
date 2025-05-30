#!/bin/bash
/usr/bin/g++ -I/home/jammy/sst/install/include -I/home/jammy/sst/install/include/sst/elements -I/home/jammy/sst/install/include/sst/elements/mercury -I/home/jammy/sst/install/include/sst/elements/mask-mpi -fPIC -std=c++17 -c x2.cc
/usr/bin/g++ x2.o -shared -Wl,-undefined -Wl,dynamic_lookup -o libx2-skeleton.so 
