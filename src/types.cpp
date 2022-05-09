#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "types.h"

#include <Windows.h>

namespace utility {
std::wstring Utf8ToUtf16(std::string const &utf8) {
    std::wstring result;
    if (utf8.empty()) {
        return result;
    }
    int utf16Length = MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
                                          static_cast<int>(utf8.size()), 0, 0);
    result.resize(utf16Length);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()),
                        const_cast<wchar_t *>(result.data()), utf16Length);
    return result;
}
std::string Utf16ToUtf8(std::wstring const &utf16) {
    std::string result;
    if (utf16.empty()) {
        return result;
    }
    int utf8Length = WideCharToMultiByte(
        CP_UTF8, 0, utf16.data(), static_cast<int>(utf16.size()), 0, 0, 0, 0);
    result.resize(utf8Length);
    WideCharToMultiByte(CP_UTF8, 0, utf16.data(),
                        static_cast<int>(utf16.size()),
                        const_cast<char *>(result.data()), utf8Length, 0, 0);
    return result;
}
}  // namespace utility