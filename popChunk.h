#ifndef POPCHUNK_H
#define POPCHUNK_H

#include "popOp.h"
#include "popLocal.h"
#include "popUpval.h"
#include "popConstant.h"
#include "util.h"
#include "popRegister.h"
#include <vector>
#include <iostream>
#include <cstdint>
#include <string>

namespace popLua {

	class popChunk {
	public:
		popChunk(u8 newDepth = 0);
		~popChunk();
		int getLastLine();
		void setDepth(u8 newDepth = 0);
		friend std::istream& operator >> (std::istream& in, popChunk& chunk);
		friend std::ostream& operator << (std::ostream& out, popChunk& chunk);
		std::vector<popConstant> constantArray;
		std::string getReg(u32 reg, int loc);
	private:
		u8 depth;
		char intro[0x0C];
		u32 numOps;
		u32 numOps_verify;
		std::vector<popOp> opArray;
		u32 numLocals;
		std::vector<popLocal> localArray;
		u32 numUpvals;
		std::vector<popUpval> upvalArray;
		u32 numConstants;
		
		u32 numProto;
		std::vector<popChunk> chunkArray;
		std::vector<popRegister> registerArray;
		std::vector<std::vector<popOp>> lineArray;
	};
}

#endif
