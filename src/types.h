#pragma once
#include <stdint.h>

#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>

namespace utility {
std::wstring Utf8ToUtf16(std::string const &utf8);
std::string Utf16ToUtf8(std::wstring const &utf16);

template <typename T>
inline std::string intToHexStr(T val, size_t width = sizeof(T) * 2) {
    std::stringstream ss = {};
    ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

// https://levelup.gitconnected.com/c-detection-idiom-explained-5cc7207a0067
template <typename A, typename B = void>
struct CanMakeString : std::false_type {};

template <typename A>
struct CanMakeString<A,
                     std::void_t<decltype(std::to_string(std::declval<A>()))>>
    : std::true_type {};

typedef void *HANDLE;

using byte_t = unsigned char;
#if INTPTR_MAX == INT32_MAX
#define ENVIRONMENT_32_BIT
#elif INTPTR_MAX == INT64_MAX
#define ENVIRONMENT_64_BIT
#else
#error "Environment neither 32 nor 64 bit."
#endif

#ifdef ENVIRONMENT_32_BIT
using addr_t = unsigned long;
using offset_t = long;
#else
using addr_t = unsigned long long;
using offset_t = long long;
#endif
}  // namespace utility