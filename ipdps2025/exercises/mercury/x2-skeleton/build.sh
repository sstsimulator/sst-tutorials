#!/bin/bash
/usr/bin/g++ -I/Users/jpkenny/install/sst-elements-hg-fixpartition2-mpi/include/sst/elements/mercury/ -I/Users/jpkenny/install/sst-core-devel-mpi/include -I/Users/jpkenny/install/sst-elements-hg-fixpartition2-mpi/include/sst/elements -I/Users/jpkenny/install/sst-elements-hg-fixpartition2-mpi/include/sst/elements/mask-mpi -fPIC -std=c++17 -c x2.cc
/usr/bin/g++ x2.o -Wl,-undefined -Wl,dynamic_lookup  -bundle -o libx2.so 
