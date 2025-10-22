#ifndef POPFILE_H
#define POPFILE_H

#include "popChunk.h"
#include "util.h"
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

namespace popLua {
	constexpr uint8_t HEADER_SIZE = 0x17;
	
	class popFile {
	public:
		~popFile();
		int getFileSize();
		friend std::istream& operator >> (std::istream& in, popFile &file);
		friend std::ostream& operator << (std::ostream& out, popFile &file);
	private:
		char header[HEADER_SIZE];
		u32 nameSize;
		std::string name;
		popChunk *chunkArray;
		std::vector<std::string> stringArray;
	};
}

#endif
