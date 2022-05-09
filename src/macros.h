#pragma once

#define ONOFF_FPTR(funcName)                                                   \
    void funcName##On() { funcName##Func.installDetours(funcName##Callback); } \
    void funcName##Off() { funcName##Func.removeDetours(); }

#define ONOFF_LAMBDA(funcName)                         \
    void funcName##On() { funcName.installDetours(); } \
    void funcName##Off() { funcName.removeDetours(); }

#define STRING(s) #s

#define ONOFF_FPTR_COUT(funcName)                            \
    void funcName##On() {                                    \
        funcName##Func.installDetours(funcName##Callback);   \
        std::cout << #funcName " " STRING(ON) << std::endl;  \
    }                                                        \
    void funcName##Off() {                                   \
        funcName##Func.removeDetours();                      \
        std::cout << #funcName " " STRING(OFF) << std::endl; \
    }

#define ONOFF_PATCH(patchName)                              \
    void patchName##On() { patchName##Patch.applyPatch(); } \
    void patchName##Off() { patchName##Func.restoreOriginal(); }

#define ONOFF_PATCH_COUT(patchName)                           \
    void patchName##On() {                                    \
        patchName##Patch.applyPatch();                        \
        std::cout << #patchName " " STRING(ON) << std::endl;  \
    }                                                         \
    void patchName##Off() {                                   \
        patchName##Patch.restoreOriginal();                   \
        std::cout << #patchName " " STRING(OFF) << std::endl; \
    }

#define Q(A) (#A[0])

#define DISPATCH_ADD_CHAR(dispatcher, key, funcName, description) \
    dispatcher##.add(Q(key), funcName##On, funcName##Off, #description);

#define DISPATCH_ADD_KEYCODE(dispatcher, key, funcName, description)  \
    dispatcher##.add(key, funcName##On, funcName##Off, #description); \
    \