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



#ifndef _H_SST_JUNO_ASM_PROGRAM
#define _H_SST_JUNO_ASM_PROGRAM

#include "junoopcodes.h"
#include "asmlocalopcodes.h"

#include <vector>
#include <map>
#include <cinttypes>
#include <cstdint>

namespace SST {
namespace Juno {
namespace Assembler {

class AssemblyProgram {

public:
	AssemblyProgram() {
		op = new std::vector<AssemblyOperation*>();
	}

	~AssemblyProgram() {
		delete op;
	}

	std::vector<AssemblyOperation*>* getOperations() {
		return op;
	}

	uint64_t countOperations() {
		return static_cast<uint64_t>(op->size());
	}

	void addOperation( AssemblyOperation* newOp ) {
		op->push_back( newOp );
	}

	void collectLiterals() {
		printf("Collecting literals...\n");

		int64Literals.clear();

		for( auto opItr = op->begin(); opItr != op->end(); opItr++ ) {
			AssemblyOperation* curOp = (*opItr);

			printf("INSTRUCTION{%20s][%d operands]\n", curOp->getInstCode().c_str(),
				curOp->countOperands());

			for( int i = 0 ; i < curOp->countOperands(); ++i ) {
				printf("-> Operand: Type=%d\n", (int) curOp->getOperand(i)->getType());

				if( curOp->getOperand(i)->getType() == LITERAL_OPERAND ) {
					AssemblyLiteralOperand* litOp = dynamic_cast<AssemblyLiteralOperand*>( curOp->getOperand(i) );

					if( NULL == litOp ) {
						fprintf(stderr, "Error: expected a literal operand but cast failed.\n");
						exit(-1);
					}

					const int64_t litVal = litOp->getLiteral();

					printf("Searching for literal value: %" PRId64 "...\n", litVal);

					int index = -1;

					for( int j = 0; j < int64Literals.size(); ++j ) {
						if( litVal == int64Literals[j] ) {
							index = j;
							break;
						}
					}

					printf("Index: %d\n", index);

					// Found a unique literal value
					if( -1 == index ) {
						int64Literals.push_back( litVal );
					}
				}
			}
		}

		printf("Literal collection done (collected: %" PRIu64 " literals)\n",
			static_cast<uint64_t>(int64Literals.size()));
	}

	void addLabel( const std::string labelText, const uint64_t loc ) {
		auto checkExists = labelMap.find(labelText);

		if( checkExists == labelMap.end() ) {
			labelMap.insert( std::pair<std::string, uint64_t>(labelText, loc) );
		} else {
			fprintf(stderr, "Error: label \"%s\" is defined twice in this program.\n", labelText.c_str() );
			exit(-1);
		}
	}

	bool verifyLabels() {
		printf("Verifying labels...\n");
		bool foundAll = true;

		for( auto opItr = op->begin(); opItr != op->end(); opItr++ ) {
			AssemblyOperation* curOp = (*opItr);

			for( int j = 0; j < curOp->countOperands(); ++j ) {
				if( LABEL_OPERAND == curOp->getOperand(j)->getType() ) {
					AssemblyLabelOperand* labelOp = dynamic_cast<AssemblyLabelOperand*>( curOp->getOperand(j) );

					if( NULL == labelOp ) {
						fprintf(stderr, "Error: attempted to convert a LABEL_OPERAND to object but failed.\n");
						exit(-1);
					}

					const std::string& labelStr = labelOp->getLabel();

					if( labelMap.find(labelStr) == labelMap.end() ) {
						fprintf(stderr, "Error: did not find label \"%s\" in program.\n", labelStr.c_str());
						foundAll = false;
					}
				}
			}
		}

		printf("Completed checking labels\n");

		if( ! foundAll ) {
			fprintf(stderr, "Error: could not find all labels in the program.\n");
			exit(-1);
		}

		return true;
	}

	int countInt64Literals() {
		return static_cast<int>( int64Literals.size() );
	}

	int64_t getInt64Literal( const int index ) {
		return int64Literals.at( index );
	}

	void writeBinary( FILE* binary ) {
		const int64_t  BINARY_VERSION = 1000000;
		const uint64_t literalSize = static_cast<uint64_t>(int64Literals.size()) * 8;
		const uint64_t programSize = static_cast<uint64_t>(op->size()) * 4;

		// Write out the header file
		fwrite( &BINARY_VERSION, sizeof(BINARY_VERSION), 1, binary );
		fwrite( &literalSize, sizeof(literalSize), 1, binary );
		fwrite( &programSize, sizeof(programSize), 1, binary );

		for( int i = 0; i < int64Literals.size(); ++i ) {
			fwrite( &int64Literals[i], sizeof(int64_t), 1, binary );
		}

		char binaryOp[4];

		for( int i = 0; i < op->size(); ++i ) {
			for( int j = 0; j < 4; ++j ) {
				binaryOp[j] = 0;
			}

			AssemblyOperation* curOp = op->at(i);

			if( curOp->getInstCode() == "ADD" ) {
				generateBinaryOperand( JUNO_ADD, curOp, binaryOp );
			} else if( curOp->getInstCode() == "SUB" ) {
				generateBinaryOperand( JUNO_SUB, curOp, binaryOp );
			} else if( curOp->getInstCode() == "MUL" ) {
				generateBinaryOperand( JUNO_MUL, curOp, binaryOp );
			} else if( curOp->getInstCode() == "DIV" ) {
				generateBinaryOperand( JUNO_DIV, curOp, binaryOp );
			} else if( curOp->getInstCode() == "MOD" ) {
				generateBinaryOperand( JUNO_MOD, curOp, binaryOp );
			} else if( curOp->getInstCode() == "AND" ) {
				generateBinaryOperand( JUNO_AND, curOp, binaryOp );
			} else if( curOp->getInstCode() == "OR" ) {
				generateBinaryOperand( JUNO_OR, curOp, binaryOp );
			} else if( curOp->getInstCode() == "XOR" ) {
				generateBinaryOperand( JUNO_XOR, curOp, binaryOp );
			} else if( curOp->getInstCode() == "NOT" ) {
				// Yes, this uses the binaryOp array but it sets unused fields to zero
				generateUnaryOperand( JUNO_NOT, curOp, binaryOp );
//			} else if( curOp->getInstCode() == "JUMP" ) {
//				generatePCRJump( JUNO_PCR_JUMP, curOp, binaryOp, static_cast<uint64_t>(i) );
			} else if( curOp->getInstCode() == "JZERO" ) {
				generatePCRRegJump( JUNO_PCR_JUMP_ZERO, curOp, binaryOp, static_cast<uint64_t>(i) );
			} else if( curOp->getInstCode() == "JLTZ" ) {
				generatePCRRegJump( JUNO_PCR_JUMP_LTZ, curOp, binaryOp, static_cast<uint64_t>(i) );
			} else if( curOp->getInstCode() == "JGTZ" ) {
				generatePCRRegJump( JUNO_PCR_JUMP_GTZ, curOp, binaryOp, static_cast<uint64_t>(i) );
			} else if( curOp->getInstCode() == "LDA" ) {
				generateLoadAddr( JUNO_LOAD_ADDR, curOp, binaryOp );
			} else if( curOp->getInstCode() == "STA" ) {
				generateLoadAddr( JUNO_STORE_ADDR, curOp, binaryOp );
			} else if( curOp->getInstCode() == "LOAD" ) {
				generateLoad( JUNO_LOAD, curOp, binaryOp );
			} else if (curOp->getInstCode() == "RAND" ) {
				generateRand( JUNO_RAND, curOp, binaryOp );
			} else if (curOp->getInstCode() == "RSEED" ) {
				generateRandSeed(JUNO_RSEED, curOp, binaryOp );
			} else if (curOp->getInstCode() == "STORE" ) {
				generateStore( JUNO_STORE, curOp, binaryOp );
			} else if( curOp->getInstCode() == "HALT" ) {
				const uint8_t junoCode = JUNO_HALT;
				const uint8_t zero     = 0;

				memcpy( (void*) &binaryOp[0], (void*)& junoCode, sizeof(junoCode) );
				memcpy( (void*) &binaryOp[1], (void*)& zero, sizeof(zero) );
				memcpy( (void*) &binaryOp[2], (void*)& zero, sizeof(zero) );
				memcpy( (void*) &binaryOp[3], (void*)& zero, sizeof(zero) );
			} else {
				fprintf(stderr, "Unknown instruction code: %s at instruction index: %d\n",
					curOp->getInstCode().c_str(), i);
				exit(-1);
			}

			fwrite( &binaryOp[0], sizeof(char), 4, binary );
		}
	}

	void generateRandSeed( const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp ) {
		if( curOp->countOperands() != 1 ) {
			fprintf(stderr, "Error: rseed-instruction must one operand\n");
			exit(-1);
		}

		if( curOp->getOperand(0)->getType() != REGISTER_OPERAND ) {
			fprintf(stderr, "Error: rseed-instruction can only have register operands.\n");
			exit(-1);
		}

		AssemblyRegisterOperand* regOpOne = dynamic_cast<AssemblyRegisterOperand*>( curOp->getOperand(0) );
		const uint8_t regOne = regOpOne->getRegister();

		const uint64_t regOne64 = static_cast<uint64_t>(regOne);

		uint32_t finalInst = static_cast<uint32_t>(junoCode + (regOne64 << 24));
		memcpy( (void*) &binaryOp[0], (void*) &finalInst, sizeof(finalInst) );
	}

	void generateRand( const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp ) {
		if( curOp->countOperands() != 1 ) {
			fprintf(stderr, "Error: rand-instruction must one operand\n");
			exit(-1);
		}

		if( curOp->getOperand(0)->getType() != REGISTER_OPERAND ) {
			fprintf(stderr, "Error: rand-instruction can only have register operands.\n");
			exit(-1);
		}

		AssemblyRegisterOperand* regOpOne = dynamic_cast<AssemblyRegisterOperand*>( curOp->getOperand(0) );
		const uint8_t regOne = regOpOne->getRegister();

		const uint64_t regOne64 = static_cast<uint64_t>(regOne);

		uint32_t finalInst = static_cast<uint32_t>(junoCode + (regOne64 << 24));
		memcpy( (void*) &binaryOp[0], (void*) &finalInst, sizeof(finalInst) );
	}

	void generateLoad( const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp ) {
		if( curOp->countOperands() != 2 ) {
			fprintf(stderr, "Error: load-instruction %s must have two operands.\n", curOp->getInstCode().c_str());
			exit(-1);
		}

		if( curOp->getOperand(0)->getType() != REGISTER_OPERAND && curOp->getOperand(1)->getType() != REGISTER_OPERAND ) {
			fprintf(stderr, "Error: load-instruction can only have register operands and at least one is non-register.\n");
			exit(-1);
		}

		AssemblyRegisterOperand* regOpOne = dynamic_cast<AssemblyRegisterOperand*>( curOp->getOperand(0) );
		AssemblyRegisterOperand* regOpTwo = dynamic_cast<AssemblyRegisterOperand*>( curOp->getOperand(1) );

		const uint8_t regOne = regOpOne->getRegister();
		const uint8_t regTwo = regOpTwo->getRegister();

		const uint64_t regOne64 = static_cast<uint64_t>(regOne);
		const uint64_t regTwo64 = static_cast<uint64_t>(regTwo);

		printf("Creating a LOAD reg1=r%" PRIu8 ", regTarget=r%" PRIu8 "\n", regOne, regTwo);

		uint32_t finalInst = static_cast<uint32_t>(junoCode) + (regTwo64 << 24) + (regOne64 << 8);
                memcpy( (void*) &binaryOp[0], (void*) &finalInst, sizeof(finalInst) );
	}

	void generateStore( const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp ) {
		if( curOp->countOperands() != 2 ) {
			fprintf(stderr, "Error: store-instruction %s must have two operands.\n", curOp->getInstCode().c_str());
			exit(-1);
		}

		if( curOp->getOperand(0)->getType() != REGISTER_OPERAND && curOp->getOperand(1)->getType() != REGISTER_OPERAND ) {
			fprintf(stderr, "Error: store-instruction can only have register operands and at least one is non-register.\n");
			exit(-1);
		}

		AssemblyRegisterOperand* regOpOne = dynamic_cast<AssemblyRegisterOperand*>( curOp->getOperand(0) );
		AssemblyRegisterOperand* regOpTwo = dynamic_cast<AssemblyRegisterOperand*>( curOp->getOperand(1) );

		const uint8_t regOne = regOpOne->getRegister();
		const uint8_t regTwo = regOpTwo->getRegister();

		const uint64_t regOne64 = static_cast<uint64_t>(regOne);
		const uint64_t regTwo64 = static_cast<uint64_t>(regTwo);

		uint32_t finalInst = static_cast<uint32_t>(junoCode) + (regTwo64 << 16) + (regOne64 << 8);
                memcpy( (void*) &binaryOp[0], (void*) &finalInst, sizeof(finalInst) );
	}

	void generateLoadAddr(const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp ) {

		if( curOp->countOperands() != 2 ) {
                        fprintf(stderr, "Error: store-instruction %s must have two operands.\n", curOp->getInstCode().c_str());
                        exit(-1);
                }

                if( curOp->getOperand(1)->getType() != REGISTER_OPERAND ) {
                        fprintf(stderr, "Error: load-instruction, second operand must be a register.\n");
                        exit(-1);
                }

                AssemblyRegisterOperand* regOp = dynamic_cast<AssemblyRegisterOperand*>(curOp->getOperand(1));

                const uint8_t loadReg = regOp->getRegister();

                if( curOp->getOperand(0)->getType() != LITERAL_OPERAND ) {
                        fprintf(stderr, "Error: load-addr, first operand must be a literal.\n");
                        exit(-1);
                }

                AssemblyLiteralOperand* litOp = dynamic_cast<AssemblyLiteralOperand*>(curOp->getOperand(0));
                uint64_t litVal  = static_cast<uint64_t>( litOp->getLiteral() );

		int index = -1;

		for( int i = 0; i < int64Literals.size(); ++i ) {
			if(int64Literals[i] == litVal) {
				index = i;
				break;
			}
		}

		if( index == -1 ) {
			fprintf(stderr, "Error: unable to find literal: %" PRId64 "\n", litVal);
			exit(-1);
		}

		uint64_t litAddr = static_cast<uint64_t>( index * 8 );
                uint64_t reg64   = static_cast<uint64_t>( loadReg );

                uint32_t finalInst = static_cast<uint32_t>(junoCode) + (reg64 << 24) + ((litAddr & 0xFFFF) << 8);
                memcpy( (void*) &binaryOp[0], (void*) &finalInst, sizeof(finalInst) );
        }

	void generateStoreAddr( const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp ) {
		if( curOp->countOperands() != 2 ) {
			fprintf(stderr, "Error: store-instruction %s must have two operands.\n", curOp->getInstCode().c_str());
			exit(-1);
		}

		if( curOp->getOperand(0)->getType() != REGISTER_OPERAND ) {
			fprintf(stderr, "Error: store-instruction, first operand must be a register.\n");
			exit(-1);
		}

		AssemblyRegisterOperand* regOp = dynamic_cast<AssemblyRegisterOperand*>(curOp->getOperand(0));

		const uint8_t storeReg = regOp->getRegister();

		if( curOp->getOperand(1)->getType() != LITERAL_OPERAND ) {
			fprintf(stderr, "Error: store-addr, second operand must be a literal.\n");
			exit(-1);
		}

		AssemblyLiteralOperand* litOp = dynamic_cast<AssemblyLiteralOperand*>(curOp->getOperand(1));
                uint64_t litVal  = static_cast<uint64_t>( litOp->getLiteral() );

		int index = -1;

		for( int i = 0; i < int64Literals.size(); ++i ) {
			if(int64Literals[i] == litVal) {
				index = i;
				break;
			}
		}

		if( index == -1 ) {
			fprintf(stderr, "Error: unable to find literal: %" PRId64 "\n", litVal);
			exit(-1);
		}

		uint64_t litAddr = static_cast<uint64_t>( index * 8 );
                uint64_t reg64   = static_cast<uint64_t>( storeReg );

		uint32_t finalInst = static_cast<uint32_t>(junoCode) + (reg64 << 8) + ((litAddr & 0xFFFF) << 16);
		memcpy( (void*) &binaryOp[0], (void*) &finalInst, sizeof(finalInst) );
	}

	void generatePCRJump( const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp, uint64_t instLoc ) {
		memcpy( (void*) &binaryOp[0], (void*) &junoCode, sizeof(junoCode) );

		if( curOp->countOperands() != 1 ) {
			fprintf(stderr, "Error: instruction %s can only have one operand (a LABEL)\n", curOp->getInstCode().c_str());
			exit(-1);
		}

		if( curOp->getOperand(0)->getType() != LABEL_OPERAND ) {
			fprintf(stderr, "Error: instruction %s does not have a label operand.\n", curOp->getInstCode().c_str());
			exit(-1);
		}

		AssemblyLabelOperand* labelOp = dynamic_cast<AssemblyLabelOperand*>(curOp->getOperand(1));

		const int64_t labelLoc = static_cast<int64_t>( labelMap.find(labelOp->getLabel())->second );
		const int64_t localDiff = static_cast<int64_t>(instLoc) - labelLoc;
		const int16_t jumpBy16b = static_cast<int16_t>( localDiff );

		memcpy( (void*) &binaryOp[2], (void*) &jumpBy16b, sizeof(jumpBy16b) );
	}

	void generatePCRRegJump( const uint8_t junoCode, AssemblyOperation* curOp, char* binaryOp, uint64_t instLoc ) {
		memcpy( (void*) &binaryOp[0], (void*) &junoCode, sizeof(junoCode) );

		if( curOp->countOperands() != 2 ) {
			fprintf(stderr, "Error: instruction (%s) must have two operands, it has %d\n",
				curOp->getInstCode().c_str(), curOp->countOperands());
			exit(-1);
		}

		if( curOp->getOperand(0)->getType() != REGISTER_OPERAND ) {
			fprintf(stderr, "Error: a performance-counter relative jump must provide a register to evaluate as its first operand.\n");
			exit(-1);
		}

		AssemblyRegisterOperand* regOp = dynamic_cast<AssemblyRegisterOperand*>(curOp->getOperand(0));
		const uint8_t reg = regOp->getRegister();

		memcpy( (void*) &binaryOp[1], (void*) &reg, sizeof(reg) );

		if( curOp->getOperand(1)->getType() != LABEL_OPERAND ) {
			fprintf(stderr, "Error: a performance-counter relative jump must provide a label to jump to.\n");
		}

		AssemblyLabelOperand* labelOp = dynamic_cast<AssemblyLabelOperand*>(curOp->getOperand(1));

		const int64_t jumpLoc = static_cast<int64_t>( labelMap.find(labelOp->getLabel())->second );
		const int64_t locDiff = jumpLoc - static_cast<int64_t>( instLoc );
		const int16_t jumpBy16b = static_cast<int16_t>( locDiff );

		printf("PCR-J-Class Generated with diff: %" PRId64 " -> (16b Encode) -> %" PRId16 "\n", locDiff, jumpBy16b);

		memcpy( (void*) &binaryOp[2], (void*) &jumpBy16b, sizeof(jumpBy16b) );
	}

	void generateBinaryOperand( const char junoCode, AssemblyOperation* curOp, char* binaryOp ) {
				memcpy( (void*) &binaryOp[0], (void*) &junoCode, sizeof(junoCode) );

				if( curOp->countOperands() != 3 ) {
					fprintf(stderr, "Error: instruction (%s) must have three operands, it only has %d\n",
						curOp->getInstCode().c_str(), curOp->countOperands());
					exit(-1);
				}

				populateBinaryOperand( curOp, &binaryOp[1] );
	}

	void generateUnaryOperand( const char junoCode, AssemblyOperation* curOp, char* binaryOp ) {
				memcpy( (void*) &binaryOp[0], (void*) &junoCode, sizeof(junoCode) );

				if( curOp->countOperands() != 2 ) {
					fprintf(stderr, "Error: instruction (%s) must have two operands, it only has %d\n",
						curOp->getInstCode().c_str(), curOp->countOperands());
					exit(-1);
				}

				populateUnaryOperand( curOp, &binaryOp[1] );
	}

	void populateUnaryOperand( AssemblyOperation* curOp, char* unaryOp ) {
		// Zero out the entire operand array, unused fields need to be set to zero
		for(int j = 0;j < 3; j++) {
			unaryOp[j] = 0;
		}

		for( int j = 0; j < 2; ++j ) {
                	AssemblyOperand* curOperand = curOp->getOperand(j);

                                        if( curOperand->getType() != REGISTER_OPERAND ) {
                                                fprintf(stderr, "Error: %s operation only allows register operands, operand %d is not a register.\n",
                                                        curOp->getInstCode().c_str(), j);
                                        }

                                        AssemblyRegisterOperand* curRegOperand = dynamic_cast<AssemblyRegisterOperand*>( curOperand );
					const uint8_t reg = curRegOperand->getRegister();

			if( j == 0 ) {
                                        memcpy( (void*) &unaryOp[j], (void*) &reg, sizeof(char) );
			} else if( j == 1 ) {
                                        memcpy( (void*) &unaryOp[j+1], (void*) &reg, sizeof(char) );
			}
               }
	}

	void populateBinaryOperand( AssemblyOperation* curOp, char* binaryOp ) {
		for( int j = 0; j < 3; ++j ) {
                	AssemblyOperand* curOperand = curOp->getOperand(j);

                                        if( curOperand->getType() != REGISTER_OPERAND ) {
                                                fprintf(stderr, "Error: %s operation only allows register operands, operand %d is not a register.\n",
                                                        curOp->getInstCode().c_str(), j);
                                        }

                                        AssemblyRegisterOperand* curRegOperand = dynamic_cast<AssemblyRegisterOperand*>( curOperand );
					const uint8_t reg = curRegOperand->getRegister();

                                        memcpy( (void*) &binaryOp[j], (void*) &reg, sizeof(char) );
                                }
	}


protected:
	std::map<std::string, uint64_t> labelMap;
	std::vector<int64_t> int64Literals;
	std::vector<AssemblyOperation*>* op;

};

}
}
}

#endif
