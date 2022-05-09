#pragma once
#include <chrono>
#include <string>

namespace utility {
class FPScounter {
   public:
    std::string getString();
    bool tickAndCanReturn();

   private:
    std::chrono::steady_clock::time_point t1;
    std::chrono::steady_clock::time_point t2;
    int frameCount = 0;
    long long duration = 0;
    std::string str = "";
    bool initialized = false;
};
}  // namespace utility