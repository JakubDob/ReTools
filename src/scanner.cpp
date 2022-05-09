#include "scanner.h"
#include "winapi_helper.h"
#include "constants.h"
#include "logger.h"
#include "types.h"

#include <psapi.h>
#include <sstream>




namespace utility {
bool Scanner::parse(std::string &signature, std::string &mask, char wildcard) {
    std::stringstream newMask;
    std::stringstream tmpStrNum;
    int tmpNum;
    char lastChar;
    Logger logger("Scanner::parse", Paths::logFilePath);
    for (size_t i = 0; i < mask.length(); i++) {
        if (mask[i] == 'x' || mask[i] == wildcard) {
            newMask << mask[i];
        } else if (mask[i] == '[') {
            if (i == 0) {
                logger.log(LogLevel::error,
                           "Wrong mask format. No char preceeding brackets []. "
                           "Signature: ",
                           hex(signature));
                return false;
            }
            lastChar = mask[i - 1];
            while (mask[++i] != ']') {
                tmpStrNum << mask[i];
                if (i == mask.length()) {
                    logger.log(LogLevel::error,
                               "parse\nWrong mask format. No closing brackets "
                               "]. Signature: ",
                               hex(signature));
                    return false;
                }
            }
            try {
                tmpNum = std::stoi(tmpStrNum.str());
            } catch (std::exception &e) {
                logger.log(LogLevel::error, "Wrong mask format.", e.what(),
                           ". Signature:", hex(signature));
                return false;
            }

            if (tmpNum < 1) {
                logger.log(LogLevel::error,
                           "Wrong mask format. Number inside the brackets is "
                           "less than 1. Signature: ",
                           hex(signature));
                return false;
            }
            tmpStrNum.str(std::string());
            while (--tmpNum > 0) {
                newMask << lastChar;
            }
        } else {
            logger.log(LogLevel::error,
                       "Wrong mask format. Invalid characters. Signature: ",
                       hex(signature));
            return false;
        }
    }
    mask = newMask.str();
    int diff = static_cast<int>(signature.length() - mask.length());
    if (diff > 0) {
        mask.append(diff, 'x');
    }
    shortSig.clear();
    size_t j = 0;
    for (size_t i = 0; i < mask.length(); i++) {
        if (mask[i] != wildcard) {
            if (j == signature.size()) {
                logger.log(LogLevel::error,
                           "Wrong mask format. Too many 'x' in the mask or the "
                           "signature is too short. Signature: ",
                           hex(signature));
                return false;
            }
            shortSig.push_back(
                SigCharIdx(i, static_cast<byte_t>(signature[j++])));
        }
    }
    if (shortSig.size() < signature.size()) {
        logger.log(LogLevel::error,
                   "Wrong mask format. Too few 'x' in the mask, not every byte "
                   "in the signature is parsed. Signature: ",
                   hex(signature));
        return false;
    }
    return true;
}

bool Scanner::compile(std::string &signature, std::string &mask,
                      char wildcard) {
    if (!parse(signature, mask, wildcard)) {
        Logger logger("Scanner::compile", Paths::logFilePath);
        logger.log(LogLevel::error,
                   "Cannot parse signature with given mask. Signature:",
                   hex(signature), ", mask:", mask);
        return false;
    }
    this->skipLengths.resize(256);
    // distance from last inner wildcard to last non wildcard character
    int innerWildcardDist = 0;
    int lastCharIdx = static_cast<int>(mask.length()) - 1;
    int lastNonWildcardIdx = lastCharIdx;

    for (int i = lastCharIdx; i >= 0 && mask[i] == wildcard;
         i--, lastNonWildcardIdx--)
        ;
    for (int i = lastNonWildcardIdx; i >= 0 && mask[i] != wildcard;
         i--, innerWildcardDist++)
        ;

    for (int i = 0; i < 256; i++) {
        skipLengths[i] = innerWildcardDist;
    }
    for (size_t i = 0; i < shortSig.size() - 1; i++) {
        if (skipLengths[shortSig[i].c] >
            lastNonWildcardIdx - shortSig[i].index) {
            skipLengths[shortSig[i].c] = lastNonWildcardIdx - shortSig[i].index;
        }
    }
    return true;
}

offset_t Scanner::findInText(std::string signature, std::string mask,
                             std::string data, char wildcard) {
    if (signature == "" || compile(signature, mask, wildcard) == false) {
        return -2;
    }
    size_t maskLen = mask.length();
    size_t shortSigLen = shortSig.size();
    size_t dataLen = data.length();
    size_t dataI = 0;
    while (dataI + maskLen <= dataLen) {
        size_t i = 0;
        for (i; i < shortSigLen; i++) {
            if (shortSig[i].c !=
                static_cast<byte_t>(data[dataI + shortSig[i].index])) {
                byte_t lastChar =
                    static_cast<byte_t>(data[dataI + shortSig.back().index]);
                dataI += skipLengths[lastChar];
                break;
            }
        }
        if (i == shortSigLen) {
            return dataI;
        }
    }
    return -1;
}

addr_t const Scanner::findInProcessMemory(std::string moduleName,
                                          std::string signature,
                                          std::string mask,
                                          offset_t offsetFromModuleBase,
                                          char wildcard) {
    HMODULE hModule = GetModuleHandle(Utf8ToUtf16(moduleName).c_str());
    if (hModule == NULL) {
        Logger logger("Scanner::findInProcessMemory", Paths::logFilePath);
        logger.log(LogLevel::error,
                   "Cannot find module with name:", moduleName);
        return 0;
    }
    if (compile(signature, mask, wildcard) == false) {
        return 0;
    }
    MODULEINFO modInfo;
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo,
                         sizeof(modInfo));
    addr_t baseAddress = reinterpret_cast<addr_t>(modInfo.lpBaseOfDll);
    DWORD imgSize = modInfo.SizeOfImage;
    addr_t maxSize = baseAddress + imgSize;
    size_t maskLen = mask.length();
    size_t shortSigLen = shortSig.size();
    addr_t dataI = baseAddress + offsetFromModuleBase;
    while (dataI + maskLen <= maxSize) {
        size_t i = 0;
        for (i; i < shortSigLen; i++) {
            if (shortSig[i].c !=
                *reinterpret_cast<byte_t *>(dataI + shortSig[i].index)) {
                byte_t lastChar =
                    *reinterpret_cast<byte_t *>(dataI + shortSig.back().index);
                dataI += skipLengths[lastChar];
                break;
            }
        }
        if (i == shortSigLen) {
            return dataI;
        }
    }
    Logger logger("Scanner::findInProcessMemory", Paths::logFilePath);
    logger.log(LogLevel::error, "Cannot find signature:", hex(signature));
    return 0;
}

addr_t const Scanner::findInOtherProcessMemory(
    std::string processName, std::string moduleName, std::string signature,
    std::string mask, offset_t offsetFromModuleBase, char wildcard) {
    HANDLE hProc = getProcessHandle(processName);
    if (hProc == nullptr) {
        Logger logger("Scanner::findInOtherProcessMemory", Paths::logFilePath);
        logger.log(LogLevel::error, "process handle is nullptr:", processName);
        return 0;
    }
    HMODULE hModule = getModuleHandleFromProc(processName, moduleName);
    if (hModule == nullptr) {
        Logger logger("Scanner::findInOtherProcessMemory", Paths::logFilePath);
        logger.log(LogLevel::error, "hModule == nullptr:", moduleName);
        return 0;
    }
    MODULEINFO modInfo;
    GetModuleInformation(hProc, hModule, &modInfo, sizeof(modInfo));
    addr_t baseAddress = reinterpret_cast<addr_t>(modInfo.lpBaseOfDll);

    if (compile(signature, mask, wildcard) == false) {
        return 0;
    }
    DWORD imgSize = modInfo.SizeOfImage;
    addr_t maxSize = baseAddress + imgSize;
    size_t maskLen = mask.length();
    size_t shortSigLen = shortSig.size();
    addr_t dataI = baseAddress + offsetFromModuleBase;
    byte_t readByte = 1;
    byte_t lastChar = 1;
    while (dataI + maskLen <= maxSize) {
        size_t i = 0;
        for (i; i < shortSigLen; i++) {
            if (!ReadProcessMemory(
                    hProc, reinterpret_cast<LPCVOID>(dataI + shortSig[i].index),
                    &readByte, 1, NULL)) {
                Logger logger("Scanner::findInOtherProcessMemory",
                              Paths::logFilePath);
                logger.log(LogLevel::error,
                           "ReadProcessMemory failed:", hex(signature));
                CloseHandle(hProc);
                return 0;
            }
            if (shortSig[i].c != readByte) {
                if (!ReadProcessMemory(hProc,
                                       reinterpret_cast<LPCVOID>(
                                           dataI + shortSig.back().index),
                                       &lastChar, 1, NULL)) {
                    Logger logger("Scanner::findInOtherProcessMemory",
                                  Paths::logFilePath);
                    logger.log(LogLevel::error,
                               "ReadProcessMemory failed:", hex(signature));
                    CloseHandle(hProc);
                    return 0;
                }
                dataI += skipLengths[lastChar];
                break;
            }
        }
        if (i == shortSigLen) {
            return dataI;
        }
    }
    Logger logger("Scanner::findInOtherProcessMemory", Paths::logFilePath);
    logger.log(LogLevel::error, "Cannot find signature:", hex(signature));
    CloseHandle(hProc);
    return 0;
}
}  // namespace utility