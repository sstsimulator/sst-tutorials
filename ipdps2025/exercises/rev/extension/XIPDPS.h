//
// _XIPDPS_H_
//
// Copyright (C) 2017-2025 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#ifndef _SST_REVCPU_XIPDPS_H_
#define _SST_REVCPU_XIPDPS_H_

#include "../RevExt.h"
#include "../RevInstHelpers.h"

namespace SST::RevCPU {

class XIPDPS : public RevExt {

  static bool inst1( const RevFeature* F, RevRegFile* R,
                     RevMem* M, const RevInst& Inst ) {
    R->SetX( Inst.Rd,
             (R->GetX<uint64_t>(Inst.rs1) * R->GetX<uint64_t>(Inst.rs2) + 4) );
    R->cost += M->RandCost( F->GetMinCost(), F->GetMaxCost() );
    R->AdvancePC( Inst );
  }

  static bool inst2( const RevFeature* F, RevRegFile* R,
                     RevMem* M, const RevInst& Inst ) {
    R->SetX( Inst.Rd, (R->GetX<uint64_t>(Inst.rs1) |
                       (R->GetX<uint64_t>(Inst.rs1) &
                        (~R->GetX<uint64_t>(Inst.rs2)))) );
    R->cost += M->RandCost( F->GetMinCost(), F->GetMaxCost() );
    R->AdvancePC( Inst );
  }

  struct XIPDPSInstDefaults : RevInstDefaults {
    XIPDPSInstDefaults() {
      SetOpcode( 0b0001011 );
      SetrdClass( RevRegClass::RegGPR );
      Setrs1Class( RevRegClass::RegGPR );
      Setrs2Class( RevRegClass::RegGPR );
    }
  };

  std::vector<RevInstEntry> XIPDPSTable = {
    XIPDPSInstDefaults().SetMnemonic("inst1 %rd, %rs1, %rs2").SetFunct3(0b000).SetFunct2or7( 0b0000000000 ).SetImplFunc(inst1),
    XIPDPSInstDefaults().SetMnemonic("inst2 %rd, %rs1, %rs2").SetFunct3(0b000).SetFunct2or7( 0b0000000001 ).SetImplFunc(inst2),
  };

public:
  XIPDPS( const RevFeature* Feature, RevMem* RevMem, SST::Output* Output )
    : RevExt( "XForza", Feature, RevMem, Output ) {
    SetTable( std::move( XIPDPSTable) );
  }
};
}

#endif
