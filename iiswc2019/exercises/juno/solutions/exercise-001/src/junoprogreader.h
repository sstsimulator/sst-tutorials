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



#ifndef _H_SST_JUNO_PROG_READER
#define _H_SST_JUNO_PROG_READER

#include <cstdio>
#include <cinttypes>
#include <cstdlib>

namespace SST {
    namespace Juno {
        
        class JunoProgramReader {
            
        public:
            JunoProgramReader( FILE* appBinary, SST::Output* output ) {
                fread( (void*) &binVersion, sizeof(binVersion), 1, appBinary );
                fread( (void*) &dataLen, sizeof(dataLen), 1, appBinary );
                fread( (void*) &instLen, sizeof(instLen), 1, appBinary );

		padding = 0;

		if( (dataLen + instLen) % 8 != 0 ) {
			padding = 8 - ((dataLen + instLen) % 8);
		}

                output->verbose(CALL_INFO, 1, 0, "Application Version:      %" PRId64 "\n", binVersion);
                output->verbose(CALL_INFO, 1, 0, "Data Length:              %" PRIu64 "\n", dataLen);
                output->verbose(CALL_INFO, 1, 0, "Inst Length:              %" PRIu64 "\n", instLen);
		output->verbose(CALL_INFO, 1, 0, "Padding                   %" PRIu64 "\n", padding);

                programBinary = (char*) malloc( sizeof(char) * (dataLen + instLen + padding));

		for( uint64_t i = 0; i < (dataLen + instLen + padding); ++i ) {
			programBinary[i] = 0;
		}

                fread( (void*) programBinary, sizeof(char), (dataLen + instLen), appBinary );

                for( uint64_t i = 0; i < dataLen; i += 8 ) {
                    int64_t tmp = 0;
                    memcpy( (void*) &tmp, (void*) &programBinary[i], sizeof(tmp) );

                    output->verbose(CALL_INFO, 2, 0, " -> %8" PRIu64 " : %" PRId64 "\n", i, tmp );
                }

            }

            ~JunoProgramReader() {
                free( programBinary );
            }

            int64_t getBinaryVersion() {
                return binVersion;
            }

	    uint64_t getPadding() {
		return padding;
	    }

            uint64_t getDataLength() {
                return dataLen;
            }

            uint64_t getInstLength() {
                return instLen;
            }

            char* getBinaryBuffer() {
                return programBinary;
            }

        protected:
            int64_t  binVersion;
            uint64_t dataLen;
            uint64_t instLen;
	    uint64_t padding;
            char* programBinary;
        };
        
    }
}

#endif
