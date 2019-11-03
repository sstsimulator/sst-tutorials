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



#ifndef _H_SST_JUNO_GEN_RAND_RESP_EVENT
#define _H_SST_JUNO_GEN_RAND_RESP_EVENT

#include <sst/core/event.h>

namespace SST {
namespace Juno {

class JunoGenerateRandRespEvent : public SST::Event {

public:
	JunoGenerateRandRespEvent() : SST::Event() {}
	JunoGenerateRandRespEvent( const int evID, const int64_t rand):
		SST::Event(), id(evID), randNum(rand) {}
	~JunoGenerateRandRespEvent() {}

	int getID() const { return id; }
	int64_t getRand() const { return randNum; }

	void serialize_order( SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & id;
		ser & randNum;
	}

	ImplementSerializable(SST::Juno::JunoGenerateRandRespEvent);
protected:
	int id;
	int64_t randNum;
};

}
}

#endif
