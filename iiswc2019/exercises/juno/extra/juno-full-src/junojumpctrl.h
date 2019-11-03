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



#ifndef _H_SST_JUNO_JUMP_CONTROL
#define _H_SST_JUNO_JUMP_CONTROL

#include <cinttypes>

#include "junocpuinst.h"
#include "junoregfile.h"

namespace SST {
    namespace Juno {
        
        void executeJumpZero( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile, uint64_t* pc ) {
            const uint8_t chkReg    = inst->getReadReg1();
            const int64_t regVal   = regFile->readReg(chkReg);
            
            const int16_t pcDiff   = inst->get16bJumpOffset() * 4;
            uint64_t pcOut         = (*pc + 4);
            
            if( static_cast<int64_t>(0) == regVal ) {
                const int64_t pcI64 = static_cast<int64_t>(*pc);
                pcOut = static_cast<uint64_t>( pcI64 + static_cast<int64_t>(pcDiff) );
            }
            
            output.verbose(CALL_INFO, 4, 0, "JZERO[r%3" PRIu8 ", offset=%" PRId16 "] (%" PRId64 ", pcIn=%" PRId64 ", pcOut=%" PRId64 ")\n",
                           chkReg, pcDiff, regVal, (*pc), pcOut);
            
            *pc = pcOut;
        };
        
        void executeJumpLTZ( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile, uint64_t* pc ) {
            const uint8_t chkReg    = inst->getReadReg1();
            const int64_t regVal   = regFile->readReg(chkReg);
            
            const int16_t pcDiff   = inst->get16bJumpOffset() * 4;
            const uint64_t pcOut   = static_cast<uint64_t>( (regVal < 0) ? static_cast<int64_t>(*pc) + static_cast<int64_t>(pcDiff) : (*pc) + 4);
            
            output.verbose(CALL_INFO, 4, 0, "JLTZ[r%3" PRIu8 ", offset=%" PRId16 "] (%" PRId64 ", pcIn=%" PRId64 ", pcOut=%" PRId64 ")\n",
                           chkReg, pcDiff, regVal, (*pc), pcOut);
            
            *pc = pcOut;
        };
        
        void executeJumpGTZ( SST::Output& output, JunoCPUInstruction* inst, JunoRegisterFile* regFile, uint64_t* pc ) {
            const uint8_t chkReg    = inst->getReadReg1();
            const int64_t regVal   = regFile->readReg(chkReg);
            
            const int16_t pcDiff   = inst->get16bJumpOffset() * 4;
            const uint64_t pcOut   = static_cast<uint64_t>( (regVal > 0) ? static_cast<int64_t>(*pc) + static_cast<int64_t>(pcDiff) : (*pc) + 4);
            
            output.verbose(CALL_INFO, 4, 0, "JGTZ[r%3" PRIu8 ", offset=%" PRId16 "] (%" PRId64 ", pcIn=%" PRId64 ", pcOut=%" PRId64 ")\n",
                           chkReg, pcDiff, regVal, (*pc), pcOut);
            
            *pc = pcOut;
        };
        
        
    }
}

#endif
