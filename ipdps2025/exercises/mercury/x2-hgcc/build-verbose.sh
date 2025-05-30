#!/bin/bash
SST_HG_VERBOSE=1 hg++ -c x2.cc
SST_HG_VERBOSE=1 hg++ x2.o -o libx2-hgcc.so 
