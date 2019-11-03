CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)
CPPFLAGS=-I./
OPTIMIZE_FLAGS=-O3

JUNO_SOURCES := $(wildcard *.cc)
INSTMGR_SOURCES := $(wildcard instmgr/*.cc)
CUSTOMINST_SRCS := $(wildcard custominst/*.cc)
RANDACC_SRCS  := $(wildcard custominst/randaccel/*.cc)

JUNO_HEADERS := $(wildcard *.h)
INSTMGR_HEADERS := $(wildcard instmgr/*.h)
CUSTOMINST_HDRS := $(wildcard custominst/*.h)
RANDACC_HDRS := $(wildcard custominst/randaccel/*.h)

JUNO_OBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))
INSTMGR_OBJS := $(patsubst %.cc,%.o,$(wildcard instmgr/*.cc))
CUSTOMINST_OBJS := $(patsubst %.cc,%.o,$(wildcard custominst/*.cc))
RANDACC_OBJS := $(patsubst %.cc,%.o,$(wildcard custominst/randaccel/*.cc))

all: libjuno.so install

libjuno.so: $(JUNO_OBJS) $(ASM_OBJS) $(INSTMGR_OBJS) $(CUSTOMINST_OBJS) $(RANDACC_OBJS)
	$(CXX) $(OPTIMIZE_FLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ *.o

%.o:%.cc $(JUNO_HEADERS) $(INSTMGR_HEADERS) $(CUSTOMINST_HDRS) $(RANDACC_HDRS)
	$(CXX) $(OPTIMIZE_FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $<

install:
	sst-register juno juno_LIBDIR=$(CURDIR)

clean:
	rm -f *.o instmgr/*.o custominst/*.o libjuno.so
