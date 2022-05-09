#pragma once

namespace utility {

inline void BaseHookFunction::setUniversalCallingConvention(
    CallingConvention cc) {
    for (auto c : children) {
        c->currentCC = cc;
    }
}

inline void BaseHookFunction::destroy(BaseHookFunction* b) {
    children.erase(std::remove(children.begin(), children.end(), b),
                   children.end());
}

template <class ReturnType>
inline HookFunction<ReturnType>::HookFunction(addr_t targetAddress)
    : BaseHookFunction(this) {
    this->baseAddress = targetAddress;
    this->targetAddress = targetAddress;
    if (targetAddress == 0) {
        if (log) {
            Logger logger("HookFunction::HookFunction", Paths::logFilePath);
            logger.log(LogLevel::error, "Address is 0");
        }
        ok = false;
    } else {
        preDetourAddress = targetAddress;
        this->moduleName = "";
    }
}

template <class ReturnType>
inline HookFunction<ReturnType>::HookFunction(std::string moduleName,
                                              offset_t offsetFromModuleBase)
    : BaseHookFunction(this) {
    this->baseAddress = getModuleBaseAddress(moduleName);
    this->targetAddress = baseAddress + offsetFromModuleBase;
    if (targetAddress == 0) {
        if (log) {
            Logger logger("HookFunction::HookFunction", Paths::logFilePath);
            logger.log(LogLevel::error,
                       "Address is 0. Offset:", offsetFromModuleBase);
        }
        ok = false;
    } else {
        preDetourAddress = targetAddress;
        this->moduleName = moduleName;
    }
}

template <class ReturnType>
inline HookFunction<ReturnType>::HookFunction(std::string moduleName,
                                              std::string signature,
                                              std::string mask,
                                              offset_t offsetFromModuleBase,
                                              char wildcard)
    : BaseHookFunction(this) {
    Scanner scanner;
    this->targetAddress = scanner.findInProcessMemory(
        moduleName, signature, mask, offsetFromModuleBase, wildcard);
    this->baseAddress = getModuleBaseAddress(moduleName);
    if (targetAddress == 0) {
        if (log) {
            Logger logger("HookFunction::HookFunction", Paths::logFilePath);
            logger.log(LogLevel::error,
                       "Address is 0. Signature:", hex(signature));
        }
        ok = false;
    } else {
        preDetourAddress = targetAddress;
        this->moduleName = moduleName;
    }
}

template <class ReturnType>
inline HookFunction<ReturnType>::~HookFunction() {
    destroy(this);
}

template <class ReturnType>
inline CallingConvention HookFunction<ReturnType>::getCC() const {
    return currentCC;
}

template <class ReturnType>
inline void HookFunction<ReturnType>::setConsole(Console& console) {
    this->console = &console;
}

template <class ReturnType>
inline void HookFunction<ReturnType>::installDetours() {
    if (!isDetoured) {
        if (hook == nullptr) {
            if (log) {
                Logger logger("HookFunction::installDetours",
                              Paths::logFilePath);
                logger.log(LogLevel::error, "Hook is nullptr,", moduleName);
                if (console) {
                    console->print(moduleName, ": Error detouring function: ",
                                   preDetourAddress, ": hook is nullptr\n");
                }
            }
            return;
        }
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&reinterpret_cast<void*&>(targetAddress), hook);
        unsigned long error = DetourTransactionCommit();
        if (error == NO_ERROR) {
            isDetoured = true;
            if (log) {
                if (console) {
                    console->print("Detoured: ", preDetourAddress, " (",
                                   moduleName, "+",
                                   preDetourAddress - baseAddress, ") -> ",
                                   reinterpret_cast<addr_t>(hook), "\n");
                }
            }
        } else {
            if (log) {
                if (console) {
                    console->print(moduleName, ": Error detouring function: ",
                                   preDetourAddress, ": (error=", error, ")\n");
                }
            }
        }
    }
}
template <class ReturnType>
inline void HookFunction<ReturnType>::installDetours(void* hook) {
    if (!isDetoured) {
        if (hook == nullptr) {
            if (log) {
                Logger logger("HookFunction::installDetours",
                              Paths::logFilePath);
                logger.log(LogLevel::error, "hook is nullptr,", moduleName);
                if (console) {
                    console->print(moduleName, ": Error detouring function: ",
                                   preDetourAddress, ": hook is nullptr\n");
                }
            }
            return;
        }
        this->hook = hook;
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&reinterpret_cast<void*&>(targetAddress), hook);
        unsigned long error = DetourTransactionCommit();
        if (error == NO_ERROR) {
            isDetoured = true;
            if (log) {
                if (console) {
                    console->print("Detoured: ", preDetourAddress, " (",
                                   moduleName, "+",
                                   preDetourAddress - baseAddress, ") -> ",
                                   reinterpret_cast<addr_t>(hook), "\n");
                }
            }
        } else {
            if (log) {
                if (console) {
                    console->print(moduleName, ": Error detouring function: ",
                                   preDetourAddress, ": (error=", error, ")\n");
                }
            }
        }
    }
}

template <class ReturnType>
inline void HookFunction<ReturnType>::removeDetours() {
    if (isDetoured) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&reinterpret_cast<void*&>(targetAddress), hook);
        unsigned long error = DetourTransactionCommit();
        if (error == NO_ERROR) {
            isDetoured = false;
            if (log) {
                if (console) {
                    console->print("Restored: ", targetAddress, " (",
                                   moduleName, "+", targetAddress - baseAddress,
                                   ")", "\n");
                }
            }
        } else {
            if (log) {
                if (console) {
                    console->print(moduleName, ": Error detaching function: ",
                                   targetAddress, ": (error=", error, ")\n");
                }
            }
            Logger logger("HookFunction::removeDetours", Paths::logFilePath);
            logger.log("Error detaching function:",
                       std::to_string(targetAddress));
        }
    }
}

template <class ReturnType>
inline bool HookFunction<ReturnType>::detoured() const {
    return isDetoured;
}

template <class ReturnType>
inline addr_t HookFunction<ReturnType>::getPreDetourAddress() const {
    return preDetourAddress;
}

template <class ReturnType>
template <typename F, typename... Args>
inline auto HookFunction<ReturnType>::callOriginal(F const hook,
                                                   Args&&... args) const {
    F original = reinterpret_cast<F>(targetAddress);
    return original(std::forward<Args>(args)...);
}

template <class ReturnType>
template <typename T>
inline void HookFunction<ReturnType>::setHook(T&& t) {
    if constexpr (is_tuple<T>::value) {
        hook = std::get<0>(t);
        currentCC = std::get<1>(t);
    } else {
        if (currentCC == CallingConvention::fastcall_cc) {
            hook = castFastcall(std::forward<T>(t));
        } else if (currentCC == CallingConvention::cdecl_cc) {
            hook = castCdecl(std::forward<T>(t));
        } else if (currentCC == CallingConvention::stdcall_cc) {
            hook = castStdcall(std::forward<T>(t));
        } else {
        }
    }
}

template <class ReturnType>
template <typename... Args>
inline ReturnType HookFunction<ReturnType>::operator()(Args... args) {
    if constexpr (std::is_void<ReturnType>::value) {
        if (currentCC == CallingConvention::fastcall_cc) {
            auto fptr = (void(__fastcall*)(Args...))(targetAddress);
            fptr(args...);
        } else if (currentCC == CallingConvention::cdecl_cc) {
            auto fptr = (void(__cdecl*)(Args...))(targetAddress);
            fptr(args...);
        } else if (currentCC == CallingConvention::stdcall_cc) {
            auto fptr = (void(__stdcall*)(Args...))(targetAddress);
            fptr(args...);
        } else {
        }
    } else {
        if (currentCC == CallingConvention::fastcall_cc) {
            auto fptr = (ReturnType(__fastcall*)(Args...))(targetAddress);
            return fptr(args...);
        } else if (currentCC == CallingConvention::cdecl_cc) {
            auto fptr = (ReturnType(__cdecl*)(Args...))(targetAddress);
            return fptr(args...);
        } else if (currentCC == CallingConvention::stdcall_cc) {
            auto fptr = (ReturnType(__stdcall*)(Args...))(targetAddress);
            return fptr(args...);
        } else {
            return 0;
        }
    }
}
}  // namespace utility