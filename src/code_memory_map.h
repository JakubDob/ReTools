#pragma once
#include "dissasembler.h"
#include "types.h"

namespace utility {
struct CallData {
    addr_t callAddress;  // address of the call instruction
    addr_t callTarget;   // location pointed by the call operand
};

class CodeMemoryMap {
   public:
    explicit CodeMemoryMap(addr_t moduleBaseAddress);
    ~CodeMemoryMap();
    bool isCode(addr_t addr);
    addr_t getBase();
    addr_t getEnd();
    size_t bufferSize();
    byte_t* buffer();
    bool read(addr_t stackPtr);
    CallData getAddressOfLastCall();
    addr_t getAddressOfLastInstruction();
    size_t getStackEntryCount(addr_t sp);

   private:
    size_t mBuffSz = 64;
    size_t mReadSize = 0;
    addr_t mReadStart = 0;
    addr_t mBase = 0;
    addr_t mEnd = 0;
    byte_t* mBuff = nullptr;
    Disassembler mZyd = {};
};
}  // namespace utility
