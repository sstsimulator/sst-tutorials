CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libsimpleCarWash.so install

libsimpleCarWash.so: simpleCarWash.cc carGenerator.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

install:
	sst-register simpleCarWash simpleCarWash_LIBDIR=$(CURDIR)

run:
	sst tests/simpleCarWash.py
clean:
	rm -rf tests/graphs libsimpleCarWash.so
	sst-register -u simpleCarWash
