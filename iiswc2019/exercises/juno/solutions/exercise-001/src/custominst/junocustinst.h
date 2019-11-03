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



#ifndef _H_SST_JUNO_CUSTOM_INST
#define _H_SST_JUNO_CUSTOM_INST

#include <sst/core/subcomponent.h>

#include "junocpuinst.h"
#include "junoregfile.h"
#include "junoldstunit.h"

namespace SST {
namespace Juno {

class JunoCustomInstructionHandler : public SST::SubComponent {

public:
	JunoCustomInstructionHandler( Component* owner, Params& params ) : SubComponent(owner) {}
	~JunoCustomInstructionHandler() {}

	virtual bool canProcessInst( const uint8_t opCode ) = 0;
	virtual int  execute( SST::Output* output, JunoCPUInstruction* inst,
		JunoRegisterFile* regFile, JunoLoadStoreUnit* loadStoreUnit,
		uint64_t* pc ) = 0;
	virtual bool isBusy() = 0;
};

}
}

#endif
