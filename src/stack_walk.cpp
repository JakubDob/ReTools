#include "stack_walk.h"

#include "types.h"

namespace utility {
#ifdef ENVIRONMENT_32_BIT
// return stack address after the func returns and pops return address
__declspec(naked) addr_t retESP() {
    __asm {
			mov eax, esp
			add eax, 4  // skip the return address
			ret
    }
}
#endif
std::vector<CallData> getAddrOfLastNCalls(addr_t stackPointer,
                                          CodeMemoryMap &mm, size_t n) {
    std::vector<CallData> calls;
    size_t stackUsed = mm.getStackEntryCount(stackPointer);
    size_t count = n < stackUsed ? n : stackUsed;
    for (size_t i = 0; (i < stackUsed) && (count > 0);
         i++, stackPointer += sizeof(addr_t)) {
        addr_t data = *reinterpret_cast<addr_t *>(stackPointer);
        if (mm.read(data)) {
            CallData lastCall = mm.getAddressOfLastCall();
            if (lastCall.callAddress && mm.isCode(lastCall.callTarget)) {
                calls.push_back(lastCall);
                count--;
            }
        }
    }
    return calls;
}
void printAddrOfLastNCalls(Console &c, addr_t stackPointer, CodeMemoryMap &mm,
                           size_t n) {
    auto calls = getAddrOfLastNCalls(stackPointer, mm, n);
    for (int i = 0; i < calls.size(); i++) {
        c.print(i + 1, ") ", calls[i].callAddress, " <= \n");
    }
    c.print("\n");
}
}  // namespace utility
