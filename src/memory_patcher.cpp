#include "memory_patcher.h"

#include "constants.h"
#include "logger.h"
#include "scanner.h"
#include "winapi_helper.h"

namespace utility {
MemoryPatcher::MemoryPatcher(std::string moduleName, std::string description)
    : moduleName(moduleName), description(description) {}

void MemoryPatcher::addCustom(std::string bytes, addr_t address,
                              offset_t offset, size_t sizeInBytes) {
    if (address == 0) {
        Logger logger("MemoryPatcher::addCustom", Paths::logFilePath);
        logger.log("Address is 0. Bytes:", hex(bytes));
        ok = false;
    } else {
        addr_t realAddr = address + offset;
        if (sizeInBytes > 0) {
            size_t diff = sizeInBytes - bytes.length();
            if (diff > 0 && diff < 128) {
                for (size_t i = 0; i < diff; i++) {
                    bytes += '\x90';
                }
            }
        }
        std::string original(bytes);
        memcpy(&original[0], reinterpret_cast<void const*>(realAddr),
               bytes.length());
        ph.push_back(PatcherHelper(realAddr, original, bytes));
    }
}
void MemoryPatcher::addCustomAtOffset(std::string bytes, offset_t offset) {
    std::string original(bytes);
    addr_t realAddr = getModuleBaseAddress(moduleName) + offset;
    memcpy(&original[0], reinterpret_cast<void const*>(realAddr),
           bytes.length());
    ph.push_back(PatcherHelper(realAddr, original, bytes));
}
void MemoryPatcher::findAndAddCustom(std::string bytes, std::string signature,
                                     offset_t offset, std::string mask,
                                     char wildcard) {
    Scanner scanner;
    auto address =
        scanner.findInProcessMemory(moduleName, signature, mask, wildcard);
    if (address == 0) {
        Logger logger("MemoryPatcher::findAndAddCustom", Paths::logFilePath);
        logger.log(LogLevel::error, "Address is 0. Signature:", hex(signature));
        ok = false;
    } else {
        addCustom(bytes, address, offset);
    }
}
void MemoryPatcher::restoreOriginal() {
    if (applied) {
        for (auto& helper : ph) {
            writeBytesToMemory(helper.address, helper.original);
        }
        applied = false;
    }
}
void MemoryPatcher::applyPatch() {
    if (!applied) {
        for (auto& helper : ph) {
            writeBytesToMemory(helper.address, helper.modified);
        }
        applied = true;
    }
}
void MemoryPatcher::addNop(int byteCount, addr_t address, offset_t offset) {
    if (address == 0) {
        Logger logger("MemoryPatcher::addNop", Paths::logFilePath);
        logger.log(LogLevel::error, "Address is 0.");
        ok = false;
    } else {
        addCustom(std::string(byteCount, '\x90'), address, offset);
    }
}
void MemoryPatcher::addNopAtOffset(int byteCount, offset_t offset) {
    addCustomAtOffset(std::string(byteCount, '\x90'), offset);
}
void MemoryPatcher::findAndAddNop(int byteCount, std::string signature,
                                  offset_t offset, std::string mask,
                                  char wildcard) {
    Scanner scanner;
    auto address =
        scanner.findInProcessMemory(moduleName, signature, mask, wildcard);
    if (address == 0) {
        Logger logger("MemoryPatcher::findAndAddNop", Paths::logFilePath);
        logger.log(LogLevel::error, "Address is 0. Signature:", hex(signature));
        ok = false;
    } else {
        addCustom(std::string(byteCount, '\x90'), address, offset);
    }
}

bool MemoryPatcher::isOn() { return applied; }

void MemoryPatcher::setConsole(Console& console) { this->console = &console; }
}  // namespace utility
