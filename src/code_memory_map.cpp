#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "code_memory_map.h"

#include <Windows.h>

namespace utility {
CodeMemoryMap::CodeMemoryMap(addr_t moduleBaseAddress) {
    mBuff = new byte_t[mBuffSz];
    mZyd.initialize();

    MEMORY_BASIC_INFORMATION info = {};
    while (VirtualQuery((void*)moduleBaseAddress, &info, sizeof(info)) &&
           info.Protect != PAGE_EXECUTE_READ) {
        moduleBaseAddress += info.RegionSize;
    }
    mBase = reinterpret_cast<addr_t>(info.BaseAddress);
    mEnd = mBase + info.RegionSize;
}
CodeMemoryMap::~CodeMemoryMap() { delete[] mBuff; }

bool CodeMemoryMap::isCode(addr_t addr) {
    return (addr >= mBase && addr < mEnd);
}
addr_t CodeMemoryMap::getBase() { return mBase; }
addr_t CodeMemoryMap::getEnd() { return mEnd; }

size_t CodeMemoryMap::bufferSize() { return mBuffSz; }
byte_t* CodeMemoryMap::buffer() { return mBuff; }
bool CodeMemoryMap::read(addr_t stackData) {
    if (isCode(stackData)) {
        mReadStart = stackData - 64;
        if (mReadStart < mBase) {
            mReadStart = mBase;
        }
        mReadSize = stackData - mReadStart;
        // the stack points to some address at the beggining of a memory region,
        // so there are no previous instructions. This is not a call then.
        if (mReadSize == 0) {
            return false;
        } else {
            memcpy(mBuff, reinterpret_cast<void*>(mReadStart), mReadSize);
            return true;
        }
    } else {
        return false;
    }
}
CallData CodeMemoryMap::getAddressOfLastCall() {
    size_t cmdSize = 0, offset = 0;
    byte_t* bufPtr = mBuff;
    size_t size = mReadSize;
    while (offset < mReadSize) {
        if (!mZyd.disassemble(bufPtr, size)) {
            cmdSize = 1;
        } else {
            cmdSize = mZyd.instrLen();
        }

        offset += cmdSize;
        bufPtr += cmdSize;
        size -= cmdSize;
    }
    addr_t prev = mReadStart + offset - cmdSize;
    mZyd.disassemble(mBuff + offset - cmdSize, mBuffSz - offset + cmdSize);
    if (mZyd.isCall()) {
        ZyanU64 callTarget;
        ZydisCalcAbsoluteAddress(&mZyd.getInstruction(), &mZyd.getOperands(),
                                 prev, &callTarget);
        CallData data;
        data.callAddress = prev;
        data.callTarget = (addr_t)callTarget;
        return data;
    }
    return {0, 0};
}

addr_t CodeMemoryMap::getAddressOfLastInstruction() {
    size_t cmdSize = 0, offset = 0;
    addr_t ret = 0;
    byte_t* bufPtr = mBuff;
    size_t size = mReadSize;
    while (offset < mReadSize) {
        if (!mZyd.disassemble(bufPtr, size)) {
            cmdSize = 1;
        } else {
            cmdSize = mZyd.instrLen();
        }

        offset += cmdSize;
        bufPtr += cmdSize;
        size -= cmdSize;
    }
    ret = mReadStart + offset - cmdSize;
    return ret;
}

size_t CodeMemoryMap::getStackEntryCount(addr_t sp) {
    MEMORY_BASIC_INFORMATION info = {};
    VirtualQuery((void*)sp, &info, sizeof(info));
    size_t stackUsed =
        (info.RegionSize - (sp - (addr_t)info.BaseAddress)) / sizeof(addr_t);
    return stackUsed;
}
}  // namespace utility
