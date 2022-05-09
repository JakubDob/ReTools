#pragma once
#include <cwchar>
#include <string>

#include "constants.h"
#include "logger.h"

namespace utility {

class Console {
   public:
    template <typename... Args>
    void print(Args&&... args) {
        (_print(args), ...);
    }
    Console(std::string consoleTitle, std::string const& clientPath);
    Console(Console const&) = delete;
    Console(Console&&) = delete;
    Console& operator=(Console) = delete;
    inline static bool isOk() { return ok; }
    void flush();
    ~Console();

    template <typename T>
    Console& operator<<(T const& o) {
        _print(o);
        return *this;
    }

   private:
    HANDLE pipe;
    std::string pipeName;
    std::string consoleTitle;
    volatile long lock = 0;
    unsigned const maxStringLength = 2048;
    inline static bool ok = false;

    void enterCritialSection();
    void leaveCritialSection();
    bool WriteFileInternal(HANDLE pipe, void const* buffer,
                           unsigned long numberOfBytesToWrite,
                           unsigned long* numberOfBytesWritten);
    bool create(std::string title, std::string const& clientPath,
                std::string const& logPath);

    template <typename T>
    void _print(T&& data) {
        unsigned long written = 0;
        enterCritialSection();
        if constexpr (std::is_same_v<std::decay_t<T>, addr_t>) {
            std::string hexNum = intToHexStr(data);
            WriteFileInternal(pipe, hexNum.c_str(),
                              static_cast<unsigned long>(hexNum.length()),
                              &written);
        } else {
            if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
                WriteFileInternal(
                    pipe, data.c_str(),
                    static_cast<unsigned long>(data.length()) * sizeof(char),
                    &written);
            } else if constexpr (std::is_same_v<std::decay_t<T>,
                                                std::wstring>) {
                std::string temp = Utf16ToUtf8(data);
                WriteFileInternal(
                    pipe, temp.c_str(),
                    static_cast<unsigned long>(temp.length()) * sizeof(char),
                    &written);
            } else if constexpr (std::is_same_v<std::decay_t<T>, char const*> ||
                                 std::is_same_v<std::decay_t<T>, char*>) {
                WriteFileInternal(pipe, data,
                                  static_cast<unsigned long>(
                                      strnlen_s(data, maxStringLength)) *
                                      sizeof(char),
                                  &written);
            } else if constexpr (std::is_same_v<std::decay_t<T>,
                                                wchar_t const*> ||
                                 std::is_same_v<std::decay_t<T>, wchar_t*>) {
                std::string temp = Utf16ToUtf8(std::wstring(data));
                WriteFileInternal(
                    pipe, temp.c_str(),
                    static_cast<unsigned long>(temp.length()) * sizeof(char),
                    &written);
            } else if constexpr (std::is_same_v<std::decay_t<T>, char>) {
                WriteFileInternal(pipe, &data, sizeof(char), &written);
            } else if constexpr (std::is_same_v<std::decay_t<T>, wchar_t>) {
                char temp = static_cast<char>(data);
                WriteFileInternal(pipe, &temp, sizeof(char), &written);
            } else if constexpr (CanMakeString<decltype(data)>::value) {
                std::string str = std::to_string(data);
                WriteFileInternal(pipe, str.c_str(),
                                  static_cast<unsigned long>(str.length()),
                                  &written);
            } else {
                Logger logger("Console _print", Paths::logFilePath);
                logger.log(LogLevel::error,
                           "Cannot print an object of type:", typeid(T).name());
            }
        }
        leaveCritialSection();
    }
};
}  // namespace utility
