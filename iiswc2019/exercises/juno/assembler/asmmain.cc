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



#include <cstdio>
#include <cstdlib>

#include "asmreader.h"
#include "asmoptions.h"
#include "asmprogram.h"

using namespace SST::Juno::Assembler;

int main(int argc, char* argv[]) {

	if( argc < 2 ) {
		fprintf(stderr, "Error: specify a file to assemble!\n");
		exit(-1);
	}

	AssemblerOptions* options = new AssemblerOptions(argc, argv);
	AssemblyReader* reader = new AssemblyReader(options);

	AssemblyProgram* program = reader->assemble();

	printf("Assembly complete, cleaning up ...\n");
	printf("Found %d literal values.\n", program->countInt64Literals() );

	bool labelsOK = program->verifyLabels();

	if( labelsOK ) {
		printf("All labels checkout OK.\n");
	} 

	printf("Writing binary...\n");
	program->writeBinary( options->getOutputFile() );

	printf("Completed.\n");

	delete program;
	delete reader;
	delete options;
}
