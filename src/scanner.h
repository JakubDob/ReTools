#pragma once
#include <string>
#include <vector>

#include "types.h"

namespace utility {
class Scanner {
   public:
    // returns index of masked signature in text string
    offset_t findInText(std::string signature, std::string mask,
                        std::string data, char wildcard = '?');
    // returns memory address of masked signature in this process
    addr_t const findInProcessMemory(std::string moduleName,
                                     std::string signature,
                                     std::string mask = "", offset_t offset = 0,
                                     char wildcard = '?');
    // returns memory address of masked signature in other process
    addr_t const findInOtherProcessMemory(
        std::string processName, std::string moduleName, std::string signature,
        std::string mask = "", offset_t offset = 0, char wildcard = '?');

   private:
    struct SigCharIdx {
        size_t index;
        byte_t c;
        SigCharIdx(size_t idx, byte_t c) : index(idx), c(c) {}
    };
    std::vector<SigCharIdx> shortSig;
    std::vector<size_t> skipLengths;

    // initialize structures
    bool compile(std::string &signature, std::string &mask, char wildcard);
    bool parse(std::string &signature, std::string &mask, char wildcard);
};
}  // namespace utility
