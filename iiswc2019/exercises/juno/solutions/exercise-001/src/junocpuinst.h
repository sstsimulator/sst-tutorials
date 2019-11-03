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



#ifndef _H_SST_JUNO_CPU_INSY_
#define _H_SST_JUNO_CPU_INSY_

#include <cinttypes>
#include "junoopcodes.h"

namespace SST {
    namespace Juno {
        
        class JunoCPUInstruction {
            
        public:
            JunoCPUInstruction( const int32_t opCode ) : op(opCode) {}
            
            ~JunoCPUInstruction() {}
            
            uint8_t getInstCode() const {
                return op & 0xFF;
            }
            
            uint8_t getReadReg1() const {
                int32_t tmp = op & 0xFF00;
                tmp >>= 8;
                
                return static_cast<uint8_t>( tmp & 0xFF );
            }
            
            uint8_t getReadReg2() const {
                int32_t tmp = op & 0xFF0000;
                tmp >>= 16;
                
                return static_cast<uint8_t>( tmp & 0xFF );
            }
            
            int16_t get16bJumpOffset() const {
                int32_t tmp = op & 0xFFFF0000;
                tmp >>= 16;
                
                return static_cast<int16_t>( tmp & 0xFFFF );
            }
            
            uint16_t get16bAbsAddr() const {
                int32_t tmp = op & 0x00FFFF00;
                tmp >>= 8;
                
                return static_cast<uint16_t>( tmp & 0xFFFF );
            }
            
            uint8_t getWriteReg() const {
                int32_t tmp = op & 0xFF000000;
                tmp >>= 24;
                
                return static_cast<uint8_t>( tmp & 0xFF );
            }
            
        protected:
            int32_t op;
            
        };
        
    }
}

#endif
