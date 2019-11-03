// Copyright 2013-2018 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2013-2018, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.



#ifndef _H_SST_JUNO_RAND_ACCEL
#define _H_SST_JUNO_RAND_ACCEL

#include <sst/core/link.h>
#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/rng/mersenne.h>

#include "junogenrandev.h"

using namespace SST::RNG;
using namespace SST::Core;

namespace SST {
namespace Juno {

class JunoRandAccelerator : public SST::Component {
public:
	JunoRandAccelerator(SST::ComponentId_t id, SST::Params& params);
	~JunoRandAccelerator();

	void handleGenerateReq(SST::Event* ev);
	void handleRNGenerated(SST::Event* ev);

	SST_ELI_REGISTER_COMPONENT(
		JunoRandAccelerator,
		"juno",
		"JunoRandAccelerator",
		SST_ELI_ELEMENT_VERSION(1, 0, 0),
		"Juno Random Number Generator Accelerator",
		COMPONENT_CATEGORY_PROCESSOR
		)

	SST_ELI_DOCUMENT_PARAMS(
		{ "seed", "Random number generator seed", "1010101" },
		{ "verbose", "Sets the level of verbosity for the output", "0" },
		{ "gentime", "Sets the generate time for the accelerator", "10ns" }
		)

	SST_ELI_DOCUMENT_PORTS(
		{ "cpulink", "Link the CPU to the accelerator", { "juno.JunoGenerateRandEvent", "" } }
		)
protected:
	SST::Output* output;
	SST::Link* cpuLink;
	SST::Link* selfGenerateLink;
	MersenneRNG* rng;

};

}
}

#endif
