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



#include <sst/core/sst_config.h>
#include <sst/core/rng/mersenne.h>

#include "custominst/randaccel/junogenrandresp.h"
#include "custominst/randaccel/junogenrandev.h"
#include "custominst/junorandextinst.h"
#include "custominst/junocustinst.h"

using namespace SST::RNG;
using namespace SST::Juno;

JunoExternalRandInstructionHandler::JunoExternalRandInstructionHandler( Component* owner, Params& params ) :
		JunoCustomInstructionHandler( owner, params ), nextEvID(0), targetReg(0) {

	randAccLink = configureLink( "genlink", "1ns", new Event::Handler<JunoExternalRandInstructionHandler>(
		this, &JunoExternalRandInstructionHandler::handleGenerateResp));

	statRandCalls = registerStatistic<uint64_t>("rand-calls");
	statCyclesBusy = registerStatistic<uint64_t>("cycles-busy");
}

JunoExternalRandInstructionHandler::~JunoExternalRandInstructionHandler() {

}

bool JunoExternalRandInstructionHandler::isBusy() {
	// Keep track of how many cycles we are busy
	statCyclesBusy->addData(1);

	// is the unit still processing old instructions?
	// we detect this by seeing if we are targetting register zero
	// since this is where the PC is located, if its zero
	// we are not currently processing anything
	return (targetReg > 0);
}

// Return true if the op-code is either RAND or RSEED instructions
// that's all we can process in this unit
bool JunoExternalRandInstructionHandler::canProcessInst( const uint8_t opCode ) {
	return (opCode == JUNO_RAND) || (opCode == JUNO_RSEED);
}

void JunoExternalRandInstructionHandler::handleGenerateResp(SST::Event* ev) {
	cpuOut->verbose(CALL_INFO, 2, 0, "Recv response from random accelerator\n");

	JunoGenerateRandRespEvent* resp = dynamic_cast<JunoGenerateRandRespEvent*>(ev);

	if( NULL == resp ) {
		cpuOut->fatal(CALL_INFO, -1, "Error: event was not a response from rand accelerator\n");
	}

	cpuOut->verbose(CALL_INFO, 2, 0, "Generated random value is: %" PRId64 "\n", resp->getRand());
	registers->writeReg( targetReg, resp->getRand() );

	// Reset the target register back to zero, this is our
	// book keeping, if register is zero we are not currently
	// processing any instructions
	cpuOut->verbose(CALL_INFO, 2, 0, "Reseting register back to zero, free CPU to process instructions.\n");
	targetReg = 0;

	// Keep a count of how many calls we make
	statRandCalls->addData(1);

	// Delete the response now
	delete ev;
}

void JunoExternalRandInstructionHandler::executeRand( SST::Output* output, const JunoCPUInstruction* inst,
	JunoRegisterFile* regFile ) {

	targetReg = inst->getWriteReg();
	output->verbose(CALL_INFO, 2, 0, "Executing custom RAND instruction: RAND[r%" PRIu8 "]\n", targetReg);

	registers = regFile;
	cpuOut = output;

	output->verbose(CALL_INFO, 2, 0, "Sending request to an external random number generator...\n");

	// Send a request to the accelerator to do some processing on our behalf
	// this really generates a random number and sends it back to us
	// to be written into the register file
	JunoGenerateRandEvent* genRand = new JunoGenerateRandEvent(nextEvID++);
	randAccLink->send(genRand);

	output->verbose(CALL_INFO, 2, 0, "Send to external generator complete.\n");
}

void JunoExternalRandInstructionHandler::executeRandSeed( SST::Output* output, const JunoCPUInstruction* inst,
        JunoRegisterFile* regFile ) {
}

int JunoExternalRandInstructionHandler::execute( SST::Output* output, JunoCPUInstruction* inst,
        JunoRegisterFile* regFile, JunoLoadStoreUnit* loadStoreUnit, uint64_t* pc ) {

	switch( inst->getInstCode() ) {
	case JUNO_RAND:
		executeRand( output, inst, regFile );
		break;

	case JUNO_RSEED:
		break;

	default:
		output->fatal(CALL_INFO, -1, "Error: unknown instruction prefix: %" PRIu8 "\n",
			inst->getInstCode());
		return 1;
	}

	(*pc) = (*pc) + 4;
	return 0;
}
