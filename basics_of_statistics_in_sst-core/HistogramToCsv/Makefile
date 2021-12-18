
CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libstatsexample.so install

libstatsexample.so: 	StatsComponent.cc\
	    				StatsComponent.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register statsexample statsexample_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libstatsexample.so
