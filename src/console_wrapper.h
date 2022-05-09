#pragma once
#include <cstdio>
namespace utility {
class ConsoleWrapper {
   public:
    ConsoleWrapper(bool wideChar = false);
    ~ConsoleWrapper();

   private:
    FILE* pFile = nullptr;
    bool ok = true;
};
}  // namespace utility
