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



#ifndef _H_SST_JUNO_ASM_READER_
#define _H_SST_JUNO_ASM_READER_

#include "asmop.h"
#include "asmprogram.h"
#include "asmoptions.h"

namespace SST {
namespace Juno {
namespace Assembler {

class AssemblyReader {

public:
	AssemblyReader(AssemblerOptions* options);
	~AssemblyReader();

	AssemblyProgram* assemble();
	int getLiteralIndex(const int64_t checkLit);
	int64_t convertLiteralFromString(const char* litStr);

protected:
	AssemblerOptions* options;
	bool readLine(char* buffer, const int buffLen);

};

}
}
}

#endif
