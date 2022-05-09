#pragma once
#include <functional>

#include "console.h"
#include "key_toggle.h"

namespace utility {
// dispatch functions based on input
class Dispatcher {
   public:
    using funcToggle = std::function<void()>;
    Dispatcher() : exitKey('`'), infoKey('i'), protectKey(0x6A) {}
    void add(char key, funcToggle on, funcToggle off = nullptr,
             std::string const description = "");
    void add(int vKey, funcToggle on, funcToggle off = nullptr,
             std::string const description = "");
    void dispatch(int const msSleepTime);
    void printInfo();
    void setInfoKey(char key) { infoKey = KeyToggle(key); };
    void setInfoKey(int vKey) { infoKey = KeyToggle(vKey); };
    void setExitKey(char key) { exitKey = KeyToggle(key); };
    void setExitKey(int vKey) { exitKey = KeyToggle(vKey); };
    void setProtectKey(char key) { protectKey = KeyToggle(key); };
    void setProtectKey(int vKey) { protectKey = KeyToggle(vKey); };
    void setConsole(Console& console);

   private:
    Console* console = nullptr;
    KeyToggle exitKey;
    KeyToggle infoKey;
    KeyToggle protectKey;

    struct InfoHelper {
        char key;
        std::string const description;
        InfoHelper(char key, std::string description)
            : key(key), description(description) {}
    };
    struct CallbackHelper {
        KeyToggle kt;
        funcToggle on;
        funcToggle off;
        explicit CallbackHelper(char key, funcToggle on,
                                funcToggle off = nullptr)
            : kt(key), on(on), off(off){};
        explicit CallbackHelper(int key, funcToggle on,
                                funcToggle off = nullptr)
            : kt(key), on(on), off(off){};
    };
    std::vector<CallbackHelper> hh;
    std::vector<InfoHelper> ih;
};
}  // namespace utility