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



#ifndef _H_SST_JUNO_ALU
#define _H_SST_JUNO_ALU

#include <cinttypes>

#include "junocpuinst.h"
#include "junoregfile.h"

namespace SST {
    namespace Juno {
        
        void executeAdd( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft + valRight;
            
            output.verbose(CALL_INFO, 4, 0, "ADD[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " + %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };

        void executeNot( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t op        = inst->getReadReg1();
            const uint8_t resultReg = inst->getWriteReg();

            const int64_t val       = regFile->readReg(op);
            const int64_t result    = ~(val);
            output.verbose(CALL_INFO, 4, 0, "NOT[r%3" PRIu8 ", res=r%3" PRIu8 "]: (~%" PRId64 " = %" PRId64 ")\n",
                           op, resultReg, val, result);

            regFile->writeReg( resultReg, result );
        };
        
        void executeSub( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft - valRight;
            
            output.verbose(CALL_INFO, 4, 0, "SUB[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " - %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };
        
        void executeDiv( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft / valRight;
            
            output.verbose(CALL_INFO, 4, 0, "DIV[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " / %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };

        void executeMod( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft % valRight;
            
            output.verbose(CALL_INFO, 4, 0, "MOD[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " mod %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };
        
        void executeMul( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft * valRight;
            
            output.verbose(CALL_INFO, 4, 0, "MUL[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " * %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };
        
        void executeAnd( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft & valRight;
            
            output.verbose(CALL_INFO, 4, 0, "AND[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " & %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };
        
        void executeOr( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft | valRight;
            
            output.verbose(CALL_INFO, 4, 0, "OR[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " | %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };
        
        void executeXor( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile ) {
            const uint8_t opLeft    = inst->getReadReg1();
            const uint8_t opRight   = inst->getReadReg2();
            const uint8_t resultReg = inst->getWriteReg();
            
            const int64_t valLeft   = regFile->readReg(opLeft);
            const int64_t valRight  = regFile->readReg(opRight);
            
            const int64_t result    = valLeft ^ valRight;
            
            output.verbose(CALL_INFO, 4, 0, "XOR[r%3" PRIu8 ", r%3" PRIu8 ", res=r%3" PRIu8 "]: (%" PRId64 " ^ %" PRId64 " = %" PRId64 ")\n",
                           opLeft, opRight, resultReg, valLeft, valRight, result);
            
            regFile->writeReg( resultReg, result );
        };
        
        
        
        
    }
}

#endif
