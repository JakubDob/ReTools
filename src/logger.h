#pragma once
#include <fstream>
#include <mutex>

#include "types.h"

namespace utility {
struct LogLevel {
    inline static struct Error {
    } error;
    inline static struct Warning {
    } warning;
    inline static struct Info { } info; };

class Logger {
   public:
    Logger(std::string const& source, std::string const& filePath)
        : source(source), filePath(filePath) {}

    template <typename... Msg>
    void log(Msg&&... msg) {
        log(LogLevel::info, std::forward<Msg>(msg)...);
    }

    template <typename... Msg>
    void log(LogLevel::Error, Msg&&... msg) {
        logImpl("ERROR", std::forward<Msg>(msg)...);
    }
    template <typename... Msg>
    void log(LogLevel::Info, Msg&&... msg) {
        logImpl("INFO", std::forward<Msg>(msg)...);
    }
    template <typename... Msg>
    void log(LogLevel::Warning, Msg&&... msg) {
        logImpl("WARNING", std::forward<Msg>(msg)...);
    }

   protected:
    /*Open and close the file on each write. Logger is used mainly for
     * debugging, no need for extra performance of keeping the file open. Also
     * no need for a singleton, just synchronize and append.
     */
    template <typename... Msg>
    void logImpl(std::string const& level, Msg&&... msg) {
        std::lock_guard lock(mutex);
        std::ofstream file(filePath, std::ios_base::app);
        time_t result = std::time(nullptr);
        tm tm;
        localtime_s(&tm, &result);
        char date[30];
        asctime_s(date, 30, &tm);
        file << date << source << " [" << level << "]: ";
        auto handleMsg = [&file]<typename Msg>(Msg const& m) {
            if (std::is_same_v<Msg, addr_t>) {
                file << std::uppercase << std::hex << m << ' ';
            } else {
                file << std::dec << m << ' ';
            }
        };
        (handleMsg(std::forward<Msg>(msg)), ...);
        file << std::endl;
    }

   private:
    inline static std::mutex mutex;
    std::string source;
    std::string filePath;
};

std::string hex(std::string data);
}  // namespace utility
