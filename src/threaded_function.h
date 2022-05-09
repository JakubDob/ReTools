#pragma once
#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "console.h"
namespace utility {
class ThreadedFunction {
   public:
    ThreadedFunction(Console &c, std::function<void()> body,
                     std::string description = "", bool infiniteLoop = false,
                     int msSleepTime = 10);
    void setBody(std::function<void()> body) { this->body = body; };
    void setDescription(std::string) { this->description = description; };
    std::function<void()> const on;
    std::function<void()> const off;

   private:
    bool infiniteLoop = false;
    int msSleepTime = 10;
    std::string description = "";
    std::function<void()> body;
    std::thread th;
    std::atomic<bool> shouldRun = false;
    Console &c;
};
}  // namespace utility