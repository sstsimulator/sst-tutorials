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



#ifndef _H_SST_JUNO_GEN_RAND_EVENT
#define _H_SST_JUNO_GEN_RAND_EVENT

#include <sst/core/event.h>

namespace SST {
namespace Juno {

class JunoGenerateRandEvent : public SST::Event {

public:
	JunoGenerateRandEvent() : SST::Event() {}
	JunoGenerateRandEvent( const int evID ):
		SST::Event(), id(evID) {}
	~JunoGenerateRandEvent() {}

	int getID() const { return id; }

	void serialize_order( SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & id;
	}

	ImplementSerializable(SST::Juno::JunoGenerateRandEvent);
protected:
	int id;
};

}
}

#endif
