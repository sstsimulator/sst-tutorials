
CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libexample.so install

libexample.so: ExampleComponent.cc\
	             ExampleComponent.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register example example_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libexample.so
