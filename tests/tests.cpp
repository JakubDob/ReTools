#include <gtest/gtest.h>

#include "src/utils.h"

using namespace utility;

_declspec(noinline) int _stdcall sumThreeInts_stdcall(int a, int b, int c) {
    return a + b + c;
}

_declspec(noinline) float _stdcall sumThreeFloats_stdcall(float a, float b,
                                                          float c) {
    return a + b + c;
}

_declspec(noinline) int _cdecl sumThreeInts_cdecl(int a, int b, int c) {
    return a + b + c;
}

_declspec(noinline) float _cdecl sumThreeFloats_cdecl(float a, float b,
                                                      float c) {
    return a + b + c;
}

_declspec(noinline) int _fastcall sumThreeInts_fastcall(int a, int b, int c) {
    return a + b + c;
}

_declspec(noinline) float _fastcall sumThreeFloats_fastcall(float a, float b,
                                                            float c) {
    return a + b + c;
}

_declspec(noinline) float _fastcall sumFiveFloats_fastcall(float a, float b,
                                                           float c, float d,
                                                           float e) {
    return a + b + c + d + e;
}

// consecutive nested calls have different return types to prevent tail
// recursive optimization (call optimized to jmp) which prevents nesting
_declspec(noinline) long long nested1(std::vector<CallData>& stack) {
    std::cout << "";
    // force runtime evaluation to prevent optimizing away the 'stack' variable
    if (stack.size() == 0) {
        return 0;
    } else {
        return 1;
    }
}

_declspec(noinline) float _cdecl nested2(std::vector<CallData>& stack) {
    std::cout << "";
    return static_cast<float>(nested1(stack));
}

_declspec(noinline) short _cdecl nested3(std::vector<CallData>& stack) {
    std::cout << "";
    return static_cast<short>(nested2(stack));
}

_declspec(noinline) double _cdecl nested4(std::vector<CallData>& stack) {
    std::cout << "";
    return static_cast<double>(nested3(stack));
}

_declspec(noinline) int _cdecl nested5(std::vector<CallData>& stack) {
    std::cout << "";
    return static_cast<int>(nested4(stack));
}

CodeMemoryMap cmm((addr_t)GetModuleHandle(NULL));

HookFunction<int> sumThreeInts_stdcallFunc((addr_t)sumThreeInts_stdcall);
HookFunction<float> sumThreeFloats_stdcallFunc((addr_t)sumThreeFloats_stdcall);
HookFunction<int> sumThreeInts_cdeclFunc((addr_t)sumThreeInts_cdecl);
HookFunction<float> sumThreeFloats_cdeclFunc((addr_t)sumThreeFloats_cdecl);
HookFunction<int> sumThreeInts_fastcallFunc((addr_t)sumThreeInts_fastcall);
HookFunction<float> sumThreeFloats_fastcallFunc((addr_t)
                                                    sumThreeFloats_fastcall);

HookFunction<int> sumThreeInts_univ_stdcallFunc((addr_t)sumThreeInts_stdcall);
HookFunction<float> sumThreeFloats_univ_stdcallFunc((addr_t)
                                                        sumThreeFloats_stdcall);
HookFunction<int> sumThreeInts_univ_cdeclFunc((addr_t)sumThreeInts_cdecl);
HookFunction<float> sumThreeFloats_univ_cdeclFunc((addr_t)sumThreeFloats_cdecl);
HookFunction<int> sumThreeInts_univ_fastcallFunc((addr_t)sumThreeInts_fastcall);
HookFunction<float> sumThreeFloats_univ_fastcallFunc(
    (addr_t)sumThreeFloats_fastcall);

HookFunction<float> sumFiveFloats_univ_fastcallFunc((addr_t)
                                                        sumFiveFloats_fastcall);

HookFunction<int> nested1_cdeclFunc((addr_t)nested1);

TEST(Hooks, sumThreeInts_stdcall) {
    sumThreeInts_stdcallFunc.setHook(stdcall_([](int a, int b, int c) {
        return sumThreeInts_stdcallFunc(a + 10, b + 20, c + 30);
    }));
    sumThreeInts_stdcallFunc.on();
    if (sumThreeInts_stdcallFunc.getCC() != CallingConvention::stdcall_cc) {
        FAIL();
    }

    bool result = sumThreeInts_stdcall(10, 20, 30) ==
                  sumThreeInts_stdcallFunc(10 + 10, 20 + 20, 30 + 30);
    if (result)
        std::cout << "";  // prevents optimisation and helps to maintain code
                          // execution order
    sumThreeInts_stdcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, testsumThreeFloats_stdcall) {
    sumThreeFloats_stdcallFunc.setHook(stdcall_([](float a, float b, float c) {
        return sumThreeFloats_stdcallFunc(a / 5.0f, b / 2.0f, c / 7.0f);
    }));
    sumThreeFloats_stdcallFunc.on();
    if (sumThreeFloats_stdcallFunc.getCC() != CallingConvention::stdcall_cc) {
        FAIL();
    }
    bool result =
        sumThreeFloats_stdcall(10.5f, 20.0f, 30.0f) ==
        sumThreeFloats_stdcallFunc(10.5f / 5.0f, 20.0f / 2.0f, 30.0f / 7.0f);
    if (result) std::cout << "";
    sumThreeFloats_stdcallFunc.off();
    ASSERT_TRUE(result);
}
TEST(Hooks, sumThreeInts_cdecl) {
    sumThreeInts_cdeclFunc.setHook(cdecl_([](int a, int b, int c) {
        return sumThreeInts_cdeclFunc(a + 10, b - 20, c + 30);
    }));
    sumThreeInts_cdeclFunc.on();
    if (sumThreeInts_cdeclFunc.getCC() != CallingConvention::cdecl_cc) {
        FAIL();
    }
    bool result = sumThreeInts_cdecl(10, 20, 30) ==
                  sumThreeInts_cdeclFunc(10 + 10, 20 - 20, 30 + 30);
    if (result) std::cout << "";
    sumThreeInts_cdeclFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeFloats_cdecl) {
    sumThreeFloats_cdeclFunc.setHook(cdecl_([](float a, float b, float c) {
        return sumThreeFloats_cdeclFunc(a / 4.0f, b / 2.0f, c / 8.0f);
    }));
    sumThreeFloats_cdeclFunc.on();
    if (sumThreeFloats_cdeclFunc.getCC() != CallingConvention::cdecl_cc) {
        FAIL();
    }
    bool result =
        sumThreeFloats_cdecl(10.5f, 20.0f, 30.0f) ==
        sumThreeFloats_cdeclFunc(10.5f / 4.0f, 20.0f / 2.0f, 30.0f / 8.0f);
    if (result) std::cout << "";
    sumThreeFloats_cdeclFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeInts_fastcall) {
    sumThreeInts_fastcallFunc.setHook(fastcall_([](int a, int b, int c) {
        return sumThreeInts_fastcallFunc(a + 10, b + 10, c + 10);
    }));
    sumThreeInts_fastcallFunc.on();
    if (sumThreeInts_fastcallFunc.getCC() != CallingConvention::fastcall_cc) {
        FAIL();
    }
    bool result = sumThreeInts_fastcall(10, 20, 30) ==
                  sumThreeInts_fastcallFunc(10 + 10, 20 + 10, 30 + 10);
    if (result) std::cout << "";
    sumThreeInts_fastcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeFloats_fastcall) {
    sumThreeFloats_fastcallFunc.setHook(
        fastcall_([](float a, float b, float c) {
            return sumThreeFloats_fastcallFunc(a / 3.0f, b / 2.0f, c / 4.0f);
        }));
    sumThreeFloats_fastcallFunc.on();
    if (sumThreeFloats_fastcallFunc.getCC() != CallingConvention::fastcall_cc) {
        FAIL();
    }
    // hooked == original with second param / 2
    bool result =
        sumThreeFloats_fastcall(10.5f, 20.0f, 30.0f) ==
        sumThreeFloats_fastcallFunc(10.5f / 3.0f, 20.0f / 2.0f, 30.0f / 4.0f);
    if (result) std::cout << "";
    sumThreeFloats_fastcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, testsumThreeInts_univ_stdcall) {
    BaseHookFunction::setUniversalCallingConvention(
        CallingConvention::stdcall_cc);
    sumThreeInts_univ_stdcallFunc.setHook([](int a, int b, int c) {
        return sumThreeInts_univ_stdcallFunc(a + 10, b - 10, c + 40);
    });
    sumThreeInts_univ_stdcallFunc.on();
    if (sumThreeInts_univ_stdcallFunc.getCC() !=
        CallingConvention::stdcall_cc) {
        FAIL();
    }
    bool result = sumThreeInts_stdcall(10, 20, 30) ==
                  sumThreeInts_univ_stdcallFunc(10 + 10, 20 - 10, 30 + 40);
    if (result) std::cout << "";
    sumThreeInts_univ_stdcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeFloats_univ_stdcall) {
    BaseHookFunction::setUniversalCallingConvention(
        CallingConvention::stdcall_cc);
    sumThreeFloats_univ_stdcallFunc.setHook([](float a, float b, float c) {
        return sumThreeFloats_univ_stdcallFunc(a / 8.0f, b / 2.0f, c * 10.0f);
    });
    sumThreeFloats_univ_stdcallFunc.on();
    if (sumThreeFloats_univ_stdcallFunc.getCC() !=
        CallingConvention::stdcall_cc) {
        FAIL();
    }
    bool result = sumThreeFloats_stdcall(10.5f, 20.0f, 30.0f) ==
                  sumThreeFloats_univ_stdcallFunc(10.5f / 8.0f, 20.0f / 2.0f,
                                                  30.0f * 10.0f);
    if (result) std::cout << "";
    sumThreeFloats_univ_stdcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeInts_univ_cdecl) {
    BaseHookFunction::setUniversalCallingConvention(
        CallingConvention::cdecl_cc);
    sumThreeInts_univ_cdeclFunc.setHook([](int a, int b, int c) {
        return sumThreeInts_univ_cdeclFunc(a + 10, b / 20, c + 5);
    });
    sumThreeInts_univ_cdeclFunc.on();
    if (sumThreeInts_univ_cdeclFunc.getCC() != CallingConvention::cdecl_cc) {
        FAIL();
    }
    bool result = sumThreeInts_cdecl(10, 20, 30) ==
                  sumThreeInts_univ_cdeclFunc(10 + 10, 20 / 20, 30 + 5);
    if (result) std::cout << "";
    sumThreeInts_univ_cdeclFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeFloats_univ_cdecl) {
    BaseHookFunction::setUniversalCallingConvention(
        CallingConvention::cdecl_cc);
    sumThreeFloats_univ_cdeclFunc.setHook([](float a, float b, float c) {
        return sumThreeFloats_univ_cdeclFunc(a * 10, b / 2.0f, c - 5);
    });
    sumThreeFloats_univ_cdeclFunc.on();
    if (sumThreeFloats_univ_cdeclFunc.getCC() != CallingConvention::cdecl_cc) {
        FAIL();
    }
    bool result =
        sumThreeFloats_cdecl(10.5f, 20.0f, 30.0f) ==
        sumThreeFloats_univ_cdeclFunc(10.5f * 10, 20.0f / 2.0f, 30.0f - 5);
    if (result) std::cout << "";
    sumThreeFloats_univ_cdeclFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeInts_univ_fastcall) {
    BaseHookFunction::setUniversalCallingConvention(
        CallingConvention::fastcall_cc);
    sumThreeInts_univ_fastcallFunc.setHook([](int a, int b, int c) {
        return sumThreeInts_univ_fastcallFunc(a + 10, b - 5, c + 10);
    });
    sumThreeInts_univ_fastcallFunc.on();
    if (sumThreeInts_univ_fastcallFunc.getCC() !=
        CallingConvention::fastcall_cc) {
        FAIL();
    }
    bool result = sumThreeInts_fastcallFunc(10, 20, 30) ==
                  sumThreeInts_univ_fastcallFunc(10 + 10, 20 - 5, 30 + 10);
    if (result) std::cout << "";
    sumThreeInts_univ_fastcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumThreeFloats_univ_fastcall) {
    BaseHookFunction::setUniversalCallingConvention(
        CallingConvention::fastcall_cc);
    sumThreeFloats_univ_fastcallFunc.setHook([](float a, float b, float c) {
        return sumThreeFloats_univ_fastcallFunc(a / 5.0f, b / 2.0f, c / 10.0f);
    });
    sumThreeFloats_univ_fastcallFunc.on();
    if (sumThreeFloats_univ_fastcallFunc.getCC() !=
        CallingConvention::fastcall_cc) {
        FAIL();
    }
    bool result = sumThreeFloats_fastcall(10.5f, 20.0f, 30.0f) ==
                  sumThreeFloats_univ_fastcallFunc(10.5f / 5.0f, 20.0f / 2.0f,
                                                   30.0f / 10.0f);
    if (result) std::cout << "";
    sumThreeFloats_univ_fastcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(Hooks, sumFiveFloats_univ_fastcall) {
    BaseHookFunction::setUniversalCallingConvention(
        CallingConvention::fastcall_cc);
    sumFiveFloats_univ_fastcallFunc.setHook(
        [](float a, float b, float c, float d, float e) {
            return sumFiveFloats_univ_fastcallFunc(a * 7.0f, b / 2.0f, c + 100,
                                                   d - 234, e / 10.0f);
        });
    sumFiveFloats_univ_fastcallFunc.on();
    if (sumFiveFloats_univ_fastcallFunc.getCC() !=
        CallingConvention::fastcall_cc) {
        FAIL();
    }

    bool result =
        sumFiveFloats_fastcall(10.5f, 20.0f, 30.0f, 40.0f, 50.0f) ==
        sumFiveFloats_univ_fastcallFunc(10.5f * 7.0f, 20.0f / 2.0f, 30.0f + 100,
                                        40.0f - 234, 50.0f / 10.0f);
    if (result) std::cout << "";
    sumFiveFloats_univ_fastcallFunc.off();
    ASSERT_TRUE(result);
}

TEST(StackWalk, traceLast5Calls) {
    std::vector<CallData> stack;
    // hook the deepest function and inspect the stack:
    // nested5.calls(nested4.calls(nested3.calls(nested2.calls(nested1))))
    nested1_cdeclFunc.setHook(cdecl_([](std::vector<CallData>& stack) {
        addr_t stackPointer;
        GET_STACK_POINTER(stackPointer);
        stack = getAddrOfLastNCalls(stackPointer, cmm, 5);
        return nested1_cdeclFunc(stack);
    }));
    nested1_cdeclFunc.on();
    nested5(stack);
    nested1_cdeclFunc.off();
    // sometimes on x86 the first found address is a wrapper function, so search
    // for the first nested function. All nested calls should be consecutive on
    // the stack.
    auto firstFn = std::find_if(
        stack.begin(), stack.end(),
        [](CallData const& d) { return d.callTarget == (addr_t)nested1; });

    ASSERT_FALSE(firstFn == stack.end());
    ASSERT_EQ(stack.size(), 5);

    std::vector<addr_t> fPtrs{(addr_t)nested1, (addr_t)nested2, (addr_t)nested3,
                              (addr_t)nested4, (addr_t)nested5};
    auto fPtrIt = fPtrs.begin();
    for (auto it = firstFn; it != stack.end(); ++it, ++fPtrIt) {
        ASSERT_EQ(it->callTarget, *fPtrIt);
    }
}
