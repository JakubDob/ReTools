#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <string>

#include "types.h"

namespace utility {
struct CALLSTACKENTRY {
    addr_t addr;
    addr_t from;
    addr_t to;
};

typedef struct {
    addr_t addr;  // addrvalue (jumps + calls)
    bool call;    // instruction is a call
} BASIC_INSTRUCTION_INFO;

void writeBytesToMemory(addr_t address, std::string bytes);
addr_t getModuleBaseAddress(std::string moduleName);
addr_t getAddrFromProcModule(std::string processName, std::string moduleName,
                             addr_t offsetFromBase);
unsigned long getProcessIdByName(std::string processName);
HANDLE getProcessHandle(std::string processName);
HANDLE getProcessHandle(unsigned long processId);
HMODULE getModuleHandleFromProc(std::string processName,
                                std::string moduleName = "");
std::string getLastErrorAsString(DWORD errorMessageID);

}  // namespace utility
