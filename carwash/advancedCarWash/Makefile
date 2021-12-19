CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libcarWash.so install

libcarWash.so: carWash.cc carGenerator.cc bay.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

install:
	sst-register carWash carWash_LIBDIR=$(CURDIR)

clean:
	rm -rf tests/graphs libcarWash.so
	sst-register -u carWash
