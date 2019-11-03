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



#ifndef _H_SST_JUNO_RAND_UNIT
#define _H_SST_JUNO_RAND_UNIT

#include <sst/core/link.h>
#include <sst/core/elementinfo.h>
#include <sst/core/subcomponent.h>
#include <sst/core/rng/mersenne.h>
#include "custominst/junocustinst.h"

#include "junoregfile.h"
#include "junoldstunit.h"

using namespace SST::RNG;

namespace SST {
namespace Juno {

#define JUNO_RAND	200
#define JUNO_RSEED	201

class JunoExternalRandInstructionHandler : public JunoCustomInstructionHandler {

public:
	JunoExternalRandInstructionHandler( Component* owner, Params& params );
	~JunoExternalRandInstructionHandler();

	// Return true if the op-code is either RAND or RSEED instructions
	// that's all we can process in this unit
	bool canProcessInst( const uint8_t opCode );
	void executeRand( SST::Output* output, const JunoCPUInstruction* inst,
                JunoRegisterFile* regFile );
	void executeRandSeed( SST::Output* output, const JunoCPUInstruction* inst,
                JunoRegisterFile* regFile );
	int execute( SST::Output* output, JunoCPUInstruction* inst,
                JunoRegisterFile* regFile, JunoLoadStoreUnit* loadStoreUnit,
		uint64_t* pc );
	bool isBusy();
	void handleGenerateResp(SST::Event* ev);

	SST_ELI_REGISTER_SUBCOMPONENT(
		JunoExternalRandInstructionHandler,
		"juno",
		"JunoExternalRandomHandler",
		SST_ELI_ELEMENT_VERSION(1, 0, 0),
		"Random number generation instruction handler that connects to an external random number generator for Juno",
		"SST::Juno::CustomInstructionHandler"
		)

	SST_ELI_DOCUMENT_PORTS(
		{ "genlink", "Link to the Random number accelerator", { "juno.JunoGenerateRandEvent", "" } }
		)

	SST_ELI_DOCUMENT_STATISTICS(
		{ "rand-calls", "Calls to the RAND instruction", "calls", 1 },
		{ "cycles-busy", "Cycles spent waiting for accelerator", "cycles", 1 }
		)

private:
	int nextEvID;
	uint8_t targetReg;
	JunoRegisterFile* registers;
	SST::Link* randAccLink;
	SST::Output* cpuOut;
	Statistic<uint64_t>* statRandCalls;
	Statistic<uint64_t>* statCyclesBusy;

};

}
}

#endif
