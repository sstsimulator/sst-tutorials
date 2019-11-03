
ASM_SOURCES := $(wildcard *.cc)
ASM_HEADERS := $(wildcard *.h)
JUNO_OP_CODES := ../src/junoopcodes.h

JUNO_OBJS := $(patsubst %.cc,%.o,$(ASM_SOURCES))

CXX=g++
CPPFLAGS=-I. -I../src
CXXFLAGS=-std=c++11 -g -O3 -Wall

sst-juno-asm: $(JUNO_OBJS) $(ASM_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(JUNO_OBJS)

%.o:%.cc $(ASM_HEADERS) $(JUNO_OP_CODES)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

clean:
	rm sst-juno-asm *.o
