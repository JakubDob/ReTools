#include "dissasembler.h"
#include "types.h"

namespace utility {
bool Disassembler::disassemble(void const* data, size_t size) {
    if (!ZYAN_SUCCESS(
            ZydisDecoderDecodeBuffer(&mDecoder, data, size, &mInstr))) {
        return false;
    }
    return true;
}

void Disassembler::initialize() {
#ifdef ENVIRONMENT_32_BIT
    ZydisDecoderInit(&mDecoder, ZYDIS_MACHINE_MODE_LEGACY_32,
                     ZYDIS_ADDRESS_WIDTH_32);
#else  // x64
    ZydisDecoderInit(&mDecoder, ZYDIS_MACHINE_MODE_LONG_64,
                     ZYDIS_ADDRESS_WIDTH_64);
#endif
}
ZydisDecodedInstruction const& Disassembler::getInstruction() { return mInstr; }
ZydisDecodedOperand const& Disassembler::getOperands() {
    return mInstr.operands[0];
}
int Disassembler::instrLen() { return mInstr.length; }

bool Disassembler::isCall() {
    return (mInstr.meta.category == ZYDIS_CATEGORY_CALL);
}
}  // namespace utility
