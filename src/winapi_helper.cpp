#include "winapi_helper.h"
#include "constants.h"
#include "logger.h"
#include "types.h"

#include <psapi.h>
#include <tlhelp32.h>
#include <string>

namespace utility {
addr_t getModuleBaseAddress(std::string moduleName) {
    MODULEINFO modInfo;
    HMODULE hModule = GetModuleHandle(Utf8ToUtf16(moduleName).c_str());
    if (hModule != NULL) {
        GetModuleInformation(GetCurrentProcess(), hModule, &modInfo,
                             sizeof(modInfo));
        return reinterpret_cast<addr_t>(modInfo.lpBaseOfDll);
    } else {
        Logger logger("getModuleBaseAddress", Paths::logFilePath);
        logger.log(LogLevel::error, "Handle to", moduleName, "is null.");
        return 0;
    }
}

void writeBytesToMemory(addr_t address, std::string bytes) {
    unsigned long oldProt;
    VirtualProtect(reinterpret_cast<void *>(address), bytes.length(),
                   PAGE_EXECUTE_READWRITE, &oldProt);
    memcpy(reinterpret_cast<void *>(address),
           static_cast<void const *>(bytes.data()), bytes.length());
    VirtualProtect(reinterpret_cast<void *>(address), bytes.length(), oldProt,
                   &oldProt);
}

unsigned long getProcessIdByName(std::string processName) {
    PROCESSENTRY32 entry{};
    HANDLE snapshot;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &entry)) {
        while (Process32Next(snapshot, &entry)) {
            if (_wcsicmp(entry.szExeFile, Utf8ToUtf16(processName).c_str()) ==
                0) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return 0;
}
HANDLE getProcessHandle(std::string processName) {
    HANDLE hProc = nullptr;
    auto pId = getProcessIdByName(processName);
    if (pId == 0) {
        Logger logger("getProcessHandle", Paths::logFilePath);
        logger.log(LogLevel::error,
                   "Cannot find process id with name:", processName);
    } else {
        hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
        if (hProc == NULL) {
            Logger logger("getProcessHandle", Paths::logFilePath);
            logger.log(LogLevel::error, "Cannot find process with id:", pId);
        }
    }
    return hProc;
}
HANDLE getProcessHandle(unsigned long processId) {
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProc == NULL) {
        Logger logger("getProcessHandle", Paths::logFilePath);
        logger.log(LogLevel::error, "Cannot find process with id:", processId);
    }
    return hProc;
}

HMODULE getModuleHandleFromProc(std::string processName,
                                std::string moduleName) {
    if (moduleName == "") {
        moduleName = processName;
    }
    HANDLE hProcess = getProcessHandle(processName);
    DWORD cbNeeded;
    HMODULE hMods[1024];
    unsigned int i;
    if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded,
                             LIST_MODULES_ALL)) {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];
            if (GetModuleBaseName(hProcess, hMods[i], szModName,
                                  sizeof(szModName) / sizeof(TCHAR))) {
                //_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
                if (_wcsicmp(szModName, Utf8ToUtf16(moduleName).c_str()) == 0) {
                    return hMods[i];
                }
            }
        }
    }
    CloseHandle(hProcess);
    return nullptr;
}
addr_t getAddrFromProcModule(std::string processName, std::string moduleName,
                             addr_t offsetFromBase) {
    HANDLE hProc = getProcessHandle(processName);
    if (hProc == nullptr) {
        Logger logger("getAddrFromProcModule", Paths::logFilePath);
        logger.log("process handle is nullptr:", processName);
        return 0;
    }
    HMODULE hModule = getModuleHandleFromProc(processName, moduleName);
    if (hModule == nullptr) {
        Logger logger("getAddrFromProcModule", Paths::logFilePath);
        logger.log("module handle is nullptr", moduleName);
        return 0;
    }
    MODULEINFO modInfo;
    GetModuleInformation(hProc, hModule, &modInfo, sizeof(modInfo));
    addr_t baseAddress = reinterpret_cast<addr_t>(modInfo.lpBaseOfDll);
    CloseHandle(hProc);

    return baseAddress + offsetFromBase;
}

// Returns the last Win32 error, in string format.
std::string getLastErrorAsString(DWORD errorMessageID) {
    if (errorMessageID == 0) return "No error message has been recorded.";

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);
    message = "Error code: " + std::to_string(errorMessageID) +
              ". Message: " + message;
    LocalFree(messageBuffer);

    return message;
}
}  // namespace utility