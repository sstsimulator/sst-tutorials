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



#ifndef _H_SST_JUNO_ASM_INST_
#define _H_SST_JUNO_ASM_INST_

#include <vector>
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <string>

namespace SST {
namespace Juno {
namespace Assembler {

enum AssemblyOperandType {
	REGISTER_OPERAND = 0,
	MEMORY_OPERAND = 1,
	LITERAL_OPERAND = 2,
	LABEL_OPERAND = 4
};

class AssemblyOperand {
public:
	AssemblyOperand() {}
	virtual AssemblyOperandType getType() = 0;
};

class AssemblyLabelOperand : public AssemblyOperand {
public:
	AssemblyLabelOperand( char* label ):
		labelText(label) {}
	~AssemblyLabelOperand() {}

	AssemblyOperandType getType() {
		return LABEL_OPERAND;
	}

	const std::string& getLabel() const {
		return labelText;
	}

protected:
	std::string labelText;
};

class AssemblyRegisterOperand : public AssemblyOperand {
	#define JUNO_MAX_REGISTER_DIGITS 6
public:
	AssemblyRegisterOperand(const uint8_t regst) :
		AssemblyOperand(), reg(regst) {}
	AssemblyRegisterOperand(const char* regst) :
		AssemblyOperand() {

		if(strlen(regst) == 0) {
			fprintf(stderr, "Error: register number is empty.\n");
			exit(-1);
		}

		if( strlen(regst) > JUNO_MAX_REGISTER_DIGITS || strlen(regst) == 1 ) {
			fprintf(stderr, "Error: register number (%s) is badly formed.\n", regst);
			exit(-1);
		}

		if( ! (regst[0] == 'r' || regst[0] == 'R') ) {
			fprintf(stderr, "Error: register badly formed - must start with r. (%s)\n", regst);
			exit(-1);
		}

		char regNum[JUNO_MAX_REGISTER_DIGITS];
		for(int i = 1; i < std::max(static_cast<size_t>(JUNO_MAX_REGISTER_DIGITS), strlen(regst)); ++i) {
			regNum[i-1] = regst[i];
		}

		reg = static_cast<uint8_t>( std::atoi(regNum) );
		printf("Register: %" PRIu8 "\n", reg);
	}

	AssemblyOperandType getType() { return REGISTER_OPERAND; }
	uint8_t getRegister() { return reg; }

protected:
	uint8_t reg;
};

class AssemblyLiteralOperand : public AssemblyOperand {
public:
	AssemblyLiteralOperand(const int64_t literal) :
		AssemblyOperand(), val(literal) {}

	AssemblyLiteralOperand(const char* litStr) :
		AssemblyOperand() {

		if(strlen(litStr) == 0) {
			fprintf(stderr, "Error: literal value is empty.\n");
			exit(-1);
		}

		if(strlen(litStr) == 1) {
			fprintf(stderr, "Error: literal value is not long enough!\n");
			exit(-1);
		}

		if( '$' != litStr[0] ) {
			fprintf(stderr, "Error: literal value does not start with a $\n");
			exit(-1);
		}

		char* literalValue = (char*) malloc( sizeof(char) * 128 );
		for(int i = 1; i < 128; ++i) {
			literalValue[i-1] = litStr[i];
		}

		val = static_cast<int64_t>( std::atoll(literalValue) );
		printf("Literal: %" PRId64 "\n", val);
	}

	AssemblyOperandType getType() { return LITERAL_OPERAND; }
	int64_t getLiteral() { return val; }

protected:
	int64_t val;
	uint64_t valIndex;
};

class AssemblyMemoryOperand : public AssemblyOperand {
public:
	AssemblyMemoryOperand(const uint64_t address) :
		AssemblyOperand(), addr(address) {}
	AssemblyMemoryOperand(const char* addrStr) :
		AssemblyOperand() {

		char* num_end;
		addr = static_cast<uint64_t>( std::strtoull(addrStr, &num_end, 10) );
	}

	uint64_t getAddress() { return addr; }
	AssemblyOperandType getType() { return MEMORY_OPERAND; }

protected:
	uint64_t addr;
};

class AssemblyOperation {

public:
	AssemblyOperation(const char* instMnu) : instCode(instMnu) {}
	~AssemblyOperation() {}

	void addOperand(AssemblyOperand* op) {
		operands.push_back(op);
	}

	int countOperands() {
		return static_cast<int>( operands.size() );
	}

	AssemblyOperand* getOperand(const int i) {
		return operands.at(i);
	}

	std::string getInstCode() {
		return instCode;
	}

protected:
	const std::string instCode;
	std::vector<AssemblyOperand*> operands;

};

}
}
}

#endif
