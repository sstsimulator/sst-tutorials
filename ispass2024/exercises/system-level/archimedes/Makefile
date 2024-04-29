CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS) -I$(shell sst-config SST_ELEMENT_LIBRARY SST_ELEMENT_LIBRARY_HOME)/include
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)


#all: libarchimedes.so install

# This sed script converts 'od' output to a comma-separated list of byte-
# values, suitable for #include'ing into an array definition.
# This can be done much more simply with xxd or hexdump, but those tools
# are not installed by default on all supported platforms.
#
# od:	-v:		Print all data
#		-t x1:	Print as byte-values, in hex
# sed:	Script 1:  Remove base-address column from od output
# 		Script 2:  Remove trailing blank line resulting from script 1
# 		Script 3:  Add '0x' prefix, and ',' suffix to each value
%.inc: %.py
	od -v -t x1 < $< | sed -e 's/^[^ ]*[ ]*//g' -e '/^\s*$$/d' -e 's/\([0-9a-f]*\)[ $$]*/0x\1,/g' > $@

libarchimedes.so: topo_ring.cc topo_ring.h pyarchimedes.inc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install: libarchimedes.so
	sst-register archimedes archimedes_LIBDIR=$(CURDIR)
	sst-register SST_ELEMENT_SOURCE archimedes=$(CURDIR)
#	sst-register SST_ELEMENT_TESTS  archimedes=$(CURDIR)/../tests

clean:
	rm -f *.o libarchimedes.so


