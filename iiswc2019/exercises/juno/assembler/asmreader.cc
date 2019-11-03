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
#include <cstring>

#include "asmop.h"
#include "asmreader.h"
#include "asmoptions.h"

using namespace SST::Juno::Assembler;

AssemblyReader::AssemblyReader( AssemblerOptions* ops ) :
	options( ops ) {}

AssemblyReader::~AssemblyReader() {}

int64_t AssemblyReader::convertLiteralFromString(const char* litStr) {
	if( strlen(litStr) < 2 ) {
		fprintf(stderr, "Error: literal string \"%s\" cannot be converted to literal value.\n", litStr);
		exit(-1);
	}

	if( '$' != litStr[0] ) {
		fprintf(stderr, "Error: literal string \"%s\" does not start with a $.\n", litStr);
		exit(-1);
	}

	return std::atoll( &litStr[1] );
}

bool AssemblyReader::readLine(char* buffer, const int buffLen) {
	FILE* inFile = options->getInputFile();

	if( feof( inFile ) ) {
		return false;
	} else {
		int nextChar = fgetc( inFile );
		int nextIndex = 0;

		if( ('\n' == nextChar) || ('\r' == nextChar) || ('\f' == nextChar) ) {

		} else {
			while( (EOF != nextChar) ) {
				buffer[nextIndex] = (char) nextChar;
				nextIndex++;

				nextChar = fgetc( inFile );

				if( ( '\n' == nextChar ) ||
				    ( '\r' == nextChar ) ||
				    ( '\f' == nextChar ) ) {

					break;
				}

				if( (nextIndex + 1) > buffLen ) {
					fprintf(stderr, "Error: assembly warning - exceeded maximum line length\n");
					break;
				}
			}
		}

		buffer[nextIndex] = '\0';

		return (nextIndex > 0);
	}
}

AssemblyProgram* AssemblyReader::assemble() {
	AssemblyProgram* program = new AssemblyProgram();

	#define JUNO_MAX_LINE_LEN 2048

	char* buffer = (char*) malloc( sizeof(char) * JUNO_MAX_LINE_LEN );
	int currentLine = 1;

	while( readLine(buffer, JUNO_MAX_LINE_LEN) ) {
		printf("Line[%s]\n", buffer);

		if( strlen(buffer) > 1 ) {
			// Allow for comments, we ignore them
			if( '#' != buffer[0] ) {
				char* inst = strtok( buffer, " " );

				if( inst[ strlen(inst) - 1 ] == ':' ) {
					// This is an assembly label
					inst[ strlen(inst) - 1 ] = '\0';

					std::string label(inst);

					printf("Creating label: \"%s\" at location: %" PRIu64 "...\n", label.c_str(), program->countOperations());
					program->addLabel(label, program->countOperations());
				} else {

				printf("Creating instruction [%s]...\n", inst);

				AssemblyOperation* newInst = new AssemblyOperation(inst);
				program->addOperation(newInst);

				printf("Populating operands...\n");

				char* op = strtok(NULL, " ");
				while( NULL != op ) {
					if( op[0] == 'r' || op[0] == 'R' ) {
						printf("Making a register from: %s\n", op);
						newInst->addOperand( new AssemblyRegisterOperand( op ) );
					} else if( op[0] == '$' ) {
						// Literal?
						printf("Making a literal from: %s\n", op);
						//const int64_t literal = convertLiteralFromString(op);

						AssemblyLiteralOperand* newLit = new AssemblyLiteralOperand( op );
						newInst->addOperand( newLit );
					} else if( isdigit( op[0] ) ) {
						printf("Making a memory from: %s\n", op);
						newInst->addOperand( new AssemblyMemoryOperand( op ) );
						// Literal?
					} else {
						// Jump Label?
						printf("Making a jump label from %s\n", op);
						newInst->addOperand( new AssemblyLabelOperand( op ) );
					}

					op = strtok( NULL, " " );
				}
				}
			}
		}

		currentLine++;
	}

	free(buffer);

	printf("Parsed all operations, found %d operations...\n",
		static_cast<int>(program->countOperations()) );

	// Collect up all the literal values we have found in the program
	// we want to have a unique set to decrease memory consumption
	// and improve cache hit rates
	program->collectLiterals();

	return program;
}

