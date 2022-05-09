#pragma once
#include <vector>

#include "code_memory_map.h"
#include "console.h"
#include "types.h"

#ifdef ENVIRONMENT_64_BIT
#include <Windows.h>
// global to not pollute the stack
static CONTEXT registerContext;
#endif  // ENVIRONMENT_64_BIT

namespace utility {
// macro to force inline
#ifdef ENVIRONMENT_32_BIT
// return stack address after the func returns and pops return address (not
// accurate)
addr_t retESP();
#define GET_STACK_POINTER(addrOut) (addrOut = retESP())
#else
#define GET_STACK_POINTER(addrOut)                     \
    ((registerContext.ContextFlags = CONTEXT_CONTROL), \
     (RtlCaptureContext(&registerContext)), (addrOut = registerContext.Rsp))
#endif
// get addresses of the last n calls within the code segment
std::vector<CallData> getAddrOfLastNCalls(addr_t stackPointer,
                                          CodeMemoryMap &mm, size_t n);
void printAddrOfLastNCalls(Console &c, addr_t stackPointer, CodeMemoryMap &mm,
                           size_t n);
}  // namespace utility