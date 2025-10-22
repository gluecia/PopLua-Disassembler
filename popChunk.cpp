#include "popChunk.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <bit>
#include <vector>

namespace popLua {

popChunk::popChunk(u8 newDepth) { depth = newDepth; }

popChunk::~popChunk() {}

int popChunk::getLastLine() {
  int lastLine = 0;

  for (u32 i = 0; i < numOps; i++) {
    popOp curOp = opArray[i];
    int curLine = curOp.getloc();
    if (curLine > lastLine) {
      lastLine = curLine;
    }
  }

  for (u32 i = 0; i < numProto; i++) {
    popChunk *curProto = &chunkArray[i];

    int curLine = curProto->getLastLine();
    if (curLine > lastLine) {
      lastLine = curLine;
    }
  }

  return lastLine;
}

std::string popChunk::getReg(u32 reg, int loc) {
  std::string result = "R(";
  result += 48 + reg;
  result += ")";
  popRegister curRegister = registerArray[reg];
  std::vector<popLocal> localArray = curRegister.localArray;

  for (std::size_t i = 0; i < localArray.size(); i++) {
    popLocal curLocal = localArray[i];
    if (loc >= curLocal.getBegin() && loc <= curLocal.getEnd()) {
      result = curLocal.getName();
      break;
    }
  }

  return result;
}

void popChunk::setDepth(u8 newDepth) { depth = newDepth; }

std::istream &operator>>(std::istream &in, popChunk &chunk) {
  std::cout << "=== CREATING NEW CHUNK ===" << std::endl;
  in.read(chunk.intro, 0x0C);

  in.read(reinterpret_cast<char *>(&chunk.numOps), sizeof(chunk.numOps));

  std::cout << "NUM OPS: " << chunk.numOps << std::endl;

  try {
    chunk.opArray.resize(chunk.numOps);
  } catch (std::exception e) {
    throw std::length_error("failed to resize chunk.opArray");
  }

  for (u32 i = 0; i < chunk.numOps; i++) {
    u32 linenum = 0;
    in.read(reinterpret_cast<char *>(&linenum), sizeof(linenum));
    chunk.opArray[i].setloc(linenum);
  }

  in.read(reinterpret_cast<char *>(&chunk.numLocals), sizeof(chunk.numLocals));
  std::cout << "NUM LOCALS: " << chunk.numLocals << std::endl;
  chunk.localArray.resize(chunk.numLocals);
  if (chunk.numLocals > 0) {
    for (u32 i = 0; i < chunk.numLocals; i++) {
      popLocal curLocal;
      u32 length;
      std::string name = "";
      char16_t tempChr;
      in.read(reinterpret_cast<char *>(&length), sizeof(length));
      for (u32 j = 0; j < length; j++) {
        in.read(reinterpret_cast<char *>(&tempChr), sizeof(char16_t));
        if (j != length - 1)
          name += tempChr; // Don't include null char
      }
      curLocal.setName(name);

      u32 begin, end;
      in.read(reinterpret_cast<char *>(&begin), sizeof(begin));
      in.read(reinterpret_cast<char *>(&end), sizeof(end));
      curLocal.setScope(begin, end);

      chunk.localArray[i] = curLocal;
      std::cout << "LOCAL NAME LENGTH: " << length << std::endl;
    }
  }

  in.read(reinterpret_cast<char *>(&chunk.numUpvals), sizeof(chunk.numUpvals));
  std::cout << "NUM UPVALS: " << chunk.numUpvals << std::endl;
  chunk.upvalArray.resize(chunk.numUpvals);
  if (chunk.numUpvals > 0) {
    for (u32 i = 0; i < chunk.numUpvals; i++) {
      popUpval curUpval;
      u32 length;

      std::string name = "";

      char tempChr;

      in.read(reinterpret_cast<char *>(&length), sizeof(length));

      for (u32 j = 0; j < length; j++) {
        in.read(reinterpret_cast<char *>(&tempChr), 1);
        name += tempChr;
      }
      curUpval.setName(name);
      chunk.upvalArray[i] = curUpval;
    }
  }

  in.read(reinterpret_cast<char *>(&chunk.numConstants),
          sizeof(chunk.numConstants));
  std::cout << "NUM CONSTANTS: " << chunk.numConstants << std::endl;
  chunk.constantArray.resize(chunk.numConstants);
  if (chunk.numConstants > 0) {
    for (u32 i = 0; i < chunk.numConstants; i++) {
      in >> chunk.constantArray[i];
    }
  }

  // Prototypes
  in.read(reinterpret_cast<char *>(&chunk.numProto), sizeof(chunk.numProto));
  chunk.chunkArray.resize(chunk.numProto);
  std::cout << "NUM PROTOTYPES: " << chunk.numProto << std::endl;
  if (chunk.numProto > 0) {
    for (u32 i = 0; i < chunk.numProto; i++) {
      char *buffer = new char[4];
      in.read(buffer, 4);
      delete[] buffer;
      in >> chunk.chunkArray[i];
      chunk.chunkArray[i].setDepth(chunk.depth + 1);
    }
  }

  // Operations
  in.read(reinterpret_cast<char *>(&chunk.numOps_verify),
          sizeof(chunk.numOps_verify));
  std::cout << "NUMOPS:       " << chunk.numOps << "\n"
            << "NUMOPSVERIFY: " << chunk.numOps_verify << std::endl;
  assert(chunk.numOps == chunk.numOps_verify);

  if (chunk.numOps > 0) {
    for (u32 i = 0; i < chunk.numOps; i++) {
      popOp *curOp = &(chunk.opArray[i]);
      (*curOp).set(in);
    }
  }

  return in;
}

std::ostream &operator<<(std::ostream &out, popChunk &chunk) {

  if (chunk.numLocals > 0) {
    for (u32 i = 0; i < chunk.depth; i++) {
      out << "   ";
    }
    out << std::setw(3) << std::right << chunk.numLocals
        << " locals: " << std::endl;
    for (u32 i = 0; i < chunk.depth; i++) {
      out << "   ";
    }
    out << "-----------SCOPE: Begin   End" << std::endl;
    for (u32 i = 0; i < chunk.numLocals; i++) {
      popLocal *curLocal = &(chunk.localArray[i]);
      for (u32 j = 0; j < chunk.depth; j++) {
        out << "   ";
      }
      out << std::setw(3) << std::right << i << ": " << std::left
          << std::setw(8) << curLocal->getName();
      out << "       " << curLocal->getBegin() << "      "
          << curLocal->getEnd();
      out << std::endl;
    }
    out << std::endl;
  }
  /*
  else {
          out << "No locals" << std::endl; //Sheerly diagnostic
  }
  */
  if (chunk.numUpvals > 0) {
    for (u32 i = 0; i < chunk.depth; i++) {
      out << "   ";
    }
    out << std::setw(3) << std::right << chunk.numUpvals
        << " upvals: " << std::endl;
    for (u32 i = 0; i < chunk.numUpvals; i++) {
      for (u32 j = 0; j < chunk.depth; j++) {
        out << "   ";
      }
      out << std::setw(3) << std::right << i << ": " << chunk.upvalArray[i]
          << std::endl;
    }
  }
  /*
  else {
          out << "No upvals" << std::endl; //Sheerly diagnostic
  }
  */
  if (chunk.numConstants > 0) {
    for (u32 i = 0; i < chunk.depth; i++) {
      out << "   ";
    }
    out << std::setw(3) << std::right << chunk.numConstants
        << " constants: " << std::endl;
  }
  /*
  else {
          out << "No constants" << std::endl; // Sheerly diagnostic
  }
  */
  for (u32 i = 0; i < chunk.numConstants; i++) {
    for (u32 j = 0; j < chunk.depth; j++) {
      out << "   ";
    }
    out << std::setw(3) << std::right << i << ": " << chunk.constantArray[i]
        << std::endl;
  }
  out << std::endl;

  // Prototypes
  if (chunk.numProto > 0) {
    for (u32 i = 0; i < chunk.depth; i++) {
      out << "   ";
    }
    out << std::setw(3) << std::right << chunk.numProto
        << " prototypes: " << std::endl
        << std::endl;
    for (u32 i = 0; i < chunk.numProto; i++) {
      for (u32 j = 0; j < chunk.depth + 1; j++) {
        out << "   ";
      }
      out << "Prototype " << std::setw(3) << std::right << i << ": "
          << std::endl
          << std::endl;
      out << chunk.chunkArray[i];
    }
  }

  // Operations

  if (chunk.numOps > 0) {
    for (u32 i = 0; i < chunk.depth; i++) {
      out << "   ";
    }
    out << std::setw(3) << std::right << chunk.numOps
        << " instructions: " << std::endl;

    for (u32 i = 0; i < chunk.numOps; i++) {
      for (u32 j = 0; j < chunk.depth; j++) {
        out << "   ";
      }
      out << std::setw(3) << std::right << i << ": " << chunk.opArray[i]
          << std::endl;
    }
    out << std::endl;
  }

  return out;
}
} // namespace popLua
