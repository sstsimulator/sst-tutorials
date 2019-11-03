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



#ifndef _H_JUNO_OP_CODES
#define _H_JUNO_OP_CODES

#define JUNO_NOOP          0
#define JUNO_LOAD          1
#define JUNO_LOAD_ADDR     2
#define JUNO_STORE         8
#define JUNO_STORE_ADDR    9

#define JUNO_ADD           16
#define JUNO_SUB           17
#define JUNO_MUL           18
#define JUNO_DIV           19
#define JUNO_MOD           20

#define JUNO_AND           32
#define JUNO_OR            33
#define JUNO_XOR           34
#define JUNO_NOT           35

//// DEPRECATED #define JUNO_PCR_JUMP      128
#define JUNO_PCR_JUMP_ZERO 129
#define JUNO_PCR_JUMP_LTZ  130
#define JUNO_PCR_JUMP_GTZ  131

#define JUNO_HALT          255

// RESERVE but kept in a subcomponent
// #define JUNO_RAND       200
// #define JUNO_RSEED      201

#endif
