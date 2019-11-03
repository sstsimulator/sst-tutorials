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



#ifndef _H_SST_JUNO_LOAD_STORE
#define _H_SST_JUNO_LOAD_STORE

#include <cinttypes>

#include "junocpuinst.h"
#include "junoregfile.h"

namespace SST {
    namespace Juno {
        
        void executeStore( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile,
                          JunoLoadStoreUnit* ldst ) {
            
            const uint8_t valReg  = inst->getReadReg1();
            const uint8_t addrReg = inst->getReadReg2();
            
            output.verbose(CALL_INFO, 4, 0, "STORE[r%" PRIu8 ", r%" PRIu8 "], (%" PRId64 ", %" PRId64 ")\n",
                           valReg, addrReg,
                           regFile->readReg(valReg), regFile->readReg(addrReg));
            
            ldst->createStoreRequest( static_cast<uint64_t>(regFile->readReg(addrReg)), valReg );
        }
        
        void executeLoad( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile,
                         JunoLoadStoreUnit* ldst ) {
            
            const uint8_t targetReg = inst->getWriteReg();
            const uint8_t addrReg   = inst->getReadReg1();
            
            output.verbose(CALL_INFO, 4, 0, "LOAD[r%5" PRIu8 ", res=r%" PRIu8 "] (%" PRId64 ")\n", targetReg,
                           addrReg, regFile->readReg( addrReg ));
            
            ldst->createLoadRequest( static_cast<uint64_t>(regFile->readReg( addrReg )), targetReg );
            
        };
        
        void executeLDA( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile, JunoLoadStoreUnit* ldst ) {
            
            const uint8_t resultReg = inst->getWriteReg();
            const uint16_t addrLit = inst->get16bAbsAddr();
            
            output.verbose(CALL_INFO, 4, 0, "LDA[%" PRIu16 ", res=r%" PRIu8 "]\n", addrLit, resultReg);
            
            ldst->createLoadRequest( static_cast<uint64_t>(addrLit), resultReg );
            
        };
        
    }
}

#endif
