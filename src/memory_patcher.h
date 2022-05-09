#pragma once

#include <functional>
#include <string>

#include "console.h"
#include "types.h"

namespace utility {
static struct Opcode {
    std::string const nop = "\x90";
    std::string const jmp = "\xEB";
    std::string const je = "\x74";
    std::string const jne = "\x75";
} Opcode;
class MemoryPatcher {
   public:
    MemoryPatcher(std::string moduleName, std::string description = "");
    void addCustom(std::string bytes, addr_t address, offset_t offset = 0,
                   size_t sizeInBytes = 0);
    void addCustomAtOffset(std::string bytes, offset_t offset);
    void findAndAddCustom(std::string bytes, std::string signature,
                          offset_t offset = 0, std::string mask = "",
                          char wildcard = '?');
    void addNop(int byteCount, addr_t address, offset_t offset = 0);
    void addNopAtOffset(int byteCount, offset_t offset);
    void findAndAddNop(int byteCount, std::string signature,
                       offset_t offset = 0, std::string mask = "",
                       char wildcard = '?');
    void applyPatch();
    void restoreOriginal();
    bool isOn();
    void setConsole(Console& console);
    static inline bool ok = true;
    std::function<void()> const on = [&]() {
        if (console && description != "")
            console->print(description, " => on\n");
        applyPatch();
    };
    std::function<void()> const off = [&]() {
        if (console && description != "")
            console->print(description, " => off\n");
        restoreOriginal();
    };

   private:
    Console* console = nullptr;
    bool applied = false;
    struct PatcherHelper {
        addr_t address;
        std::string original;
        std::string modified;
        PatcherHelper(addr_t address, std::string original,
                      std::string modified)
            : address(address), original(original), modified(modified) {}
    };
    std::vector<PatcherHelper> ph;
    std::string moduleName;
    std::string description;
};
}  // namespace utility
