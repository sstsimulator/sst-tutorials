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



#ifndef _H_SST_JUNO_FIXED_PRG_MGR
#define _H_SST_JUNO_FIXED_PRG_MGR

#include <cinttypes>
#include "junoinstmgr.h"

namespace SST {
    namespace Juno {
        
        class JunoFixedPrgInstMgr : public JunoInstructionMgr {
            
        public:
            JunoFixedPrgInstMgr( const char* buff, const uint64_t length ) :
            JunoInstructionMgr(), maxLen(length) {
                
                buffer = (char*) malloc( sizeof(char) * maxLen );
                memcpy( buffer, buff, maxLen );
            }
            ~JunoFixedPrgInstMgr() {}
            
            bool instReady( const uint64_t addr ) {
                return true;
            }
            
            JunoCPUInstruction* getInstruction( const uint64_t addr ) {
                int32_t instCode = 0;
                
                memcpy( (void*) &instCode, &buffer[addr], sizeof(instCode) );
                JunoCPUInstruction* inst = new JunoCPUInstruction( instCode);
                
                return inst;
            }
            
        protected:
            char* buffer;
            uint64_t maxLen;
            
        };
        
    }
}

#endif
