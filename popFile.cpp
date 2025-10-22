#include <cassert>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include "popFile.h"
#include "popChunk.h"

namespace popLua {

	popFile::~popFile() {
		delete[] chunkArray;
	}

	int popFile::getFileSize() {
		return chunkArray[0].getLastLine();
	}

	std::istream& operator >> (std::istream& in, popFile& file) {
		in.read(file.header, popLua::HEADER_SIZE);
		in.read(reinterpret_cast<char *>(&file.nameSize), sizeof(file.nameSize));

		if (!std::equal(std::begin(file.header), std::begin(file.header) + 5, "\x1bLuaV")) {
			throw std::runtime_error("Signature does not match the expected value.");
		}
		
		printf("%d\n", file.nameSize);
  	char16_t *buf = new char16_t[file.nameSize];
		in.read(reinterpret_cast<char *>(buf), file.nameSize * sizeof(char16_t));
		
		// THERE HAS TO BE A BETTER WAY 2 DO THIS BUT IM TOO DUMB TO FIGURE IT OUT!!!
		char *real = new char[file.nameSize];
		for (int i = 0; i < file.nameSize; i++) {
			real[i] = buf[i];
		}

		file.name = real;

		std::cout << file.name << std::endl;

  	delete[] buf;
  	delete[] real;
		file.chunkArray = new popChunk[1];
		in >> file.chunkArray[0];
		

		return in;
	}

	std::ostream& operator << (std::ostream& out, popFile& file) {
		out << file.name << std::endl << std::endl;
		out << "Main block" << std::endl << std::endl;
		out << file.chunkArray[0];
		return out;
	}
}
