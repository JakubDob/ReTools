#pragma once
#include <Zydis/Zydis.h>

namespace utility {

class Disassembler {
   public:
    bool disassemble(void const* data, size_t size);
    void initialize();
    int instrLen();
    bool isCall();
    ZydisDecodedInstruction const& getInstruction();
    ZydisDecodedOperand const& getOperands();

   private:
    ZydisDecoder mDecoder = {};
    ZydisDecodedInstruction mInstr = {};
};
}  // namespace utility
