#pragma once
#include "winapi_helper.h"
#include <detours.h>

#include <functional>
#include <string>

#include "console.h"
#include "constants.h"
#include "logger.h"
#include "scanner.h"
#include "types.h"

/*
Simulate thiscall with fastcall by passing this pointer as a first param and edx
register as a second dummy param. First param in the original metod is the third
param in the user defined hook.
*/
namespace utility {
template <typename>
struct is_tuple : std::false_type {};
template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

enum class CallingConvention { fastcall_cc, cdecl_cc, stdcall_cc };

template <typename L, typename R, typename... Args>
auto castFastcallImpl(L&& l, R (L::*mf)(Args...) const)
    -> R(__fastcall*)(Args...) {
    return l;
}
template <typename L>
auto castFastcall(L&& l) {
    return castFastcallImpl(std::forward<L>(l), &L::operator());
}
template <typename L, typename R, typename... Args>
auto castCdeclImpl(L&& l, R (L::*mf)(Args...) const) -> R(__cdecl*)(Args...) {
    return l;
}
template <typename L>
auto castCdecl(L&& l) {
    return castCdeclImpl(std::forward<L>(l), &L::operator());
}
template <typename L, typename R, typename... Args>
auto castStdcallImpl(L&& l, R (L::*mf)(Args...) const)
    -> R(__stdcall*)(Args...) {
    return l;
}
template <typename L>
auto castStdcall(L&& l) {
    return castStdcallImpl(std::forward<L>(l), &L::operator());
}

template <typename L>
auto cdecl_(L&& l) {
    return std::make_tuple(castCdecl(std::forward<L>(l)),
                           CallingConvention::cdecl_cc);
}
template <typename L>
auto stdcall_(L&& l) {
    return std::make_tuple(castStdcall(std::forward<L>(l)),
                           CallingConvention::stdcall_cc);
}
template <typename L>
auto fastcall_(L&& l) {
    return std::make_tuple(castFastcall(std::forward<L>(l)),
                           CallingConvention::fastcall_cc);
}

class BaseHookFunction {
   public:
    static inline bool isOk() { return ok; }
    static inline bool log = true;
    static void setUniversalCallingConvention(CallingConvention cc);

   protected:
    BaseHookFunction(BaseHookFunction* child) { children.push_back(child); }
    BaseHookFunction(BaseHookFunction const& o) = delete;
    BaseHookFunction(BaseHookFunction&& o) = delete;
    BaseHookFunction& operator=(BaseHookFunction const& o) = delete;
    BaseHookFunction& operator=(BaseHookFunction&& o) = delete;
    virtual ~BaseHookFunction() = default;

    static inline bool ok = true;
    CallingConvention currentCC = CallingConvention::fastcall_cc;
    void destroy(BaseHookFunction* b);

   private:
    static inline std::vector<BaseHookFunction*> children =
        std::vector<BaseHookFunction*>();
};

template <class ReturnType>
class HookFunction : BaseHookFunction {
   public:
    HookFunction(addr_t targetAddress);
    HookFunction(std::string moduleName, offset_t offsetFromModuleBase);
    HookFunction(std::string moduleName, std::string signature,
                 std::string mask = "", offset_t offsetFromModuleBase = 0,
                 char wildcard = '?');
    ~HookFunction();
    CallingConvention getCC() const;
    void setConsole(Console& console);
    void installDetours();
    void installDetours(void* hook);
    void removeDetours();
    bool detoured() const;
    addr_t getPreDetourAddress() const;

    template <typename F, typename... Args>
    auto callOriginal(F const hook, Args&&... args) const;

    template <typename T>
    void setHook(T&& t);

    template <typename... Args>
    ReturnType operator()(Args... args);

    std::function<void()> const on = [&] { installDetours(); };
    std::function<void()> const off = [&] { removeDetours(); };

   private:
    void* hook = nullptr;
    addr_t targetAddress = 0;
    addr_t preDetourAddress = 0;
    addr_t baseAddress = 0;
    std::string moduleName = "";
    bool isDetoured = false;
    Console* console = nullptr;
};
}  // namespace utility

#include "hook_function.hpp"