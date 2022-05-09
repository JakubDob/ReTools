#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "dispatcher.h"

#include <Windows.h>

#include <string>
#include <thread>

namespace utility {
void Dispatcher::add(char key, funcToggle on, funcToggle off,
                     std::string description) {
    hh.push_back(CallbackHelper(key, on, off));
    ih.push_back(InfoHelper(key, std::move(description)));
}

void Dispatcher::add(int vKey, funcToggle on, funcToggle off,
                     std::string description) {
    hh.push_back(CallbackHelper(vKey, on, off));
    ih.push_back(InfoHelper(MapVirtualKeyA(vKey, MAPVK_VK_TO_CHAR),
                            std::move(description)));
}

void Dispatcher::dispatch(int const msSleepTime) {
    std::string garbage;
    while (1) {
        if (protectKey) {
            if (protectKey.isToggled()) {
                if (console) {
                    console->print("Hotkey toggle disabled, press ",
                                   protectKey.getChar(), " to enable\n");
                }

            } else {
                if (console) {
                    console->print("Hotkey toggle enabled\n");
                }
            }
        } else if (protectKey.isToggled()) {
        } else if (exitKey) {
            for (auto& helper : hh) {
                if (helper.off != nullptr) {
                    helper.off();
                }
            }
            break;
        } else if (infoKey) {
            printInfo();
        } else {
            for (auto& helper : hh) {
                if (helper.kt) {
                    if (helper.kt.isToggled() || helper.off == nullptr) {
                        helper.on();
                    } else {
                        helper.off();
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(msSleepTime));
    }
}
void Dispatcher::printInfo() {
    if (console) {
        console->print(
            "********************************************************\n");
        console->print(exitKey.getChar(), " -> Exit\n");
        console->print(infoKey.getChar(), " -> Show this info\n");
        console->print(protectKey.getChar(), " -> Disable hotkey toggle\n");
        for (auto& helper : ih) {
            console->print(helper.key, " -> ", helper.description, "\n");
        }
        console->print(
            "********************************************************\n");
    }
}

void Dispatcher::setConsole(Console& console) { this->console = &console; }
}  // namespace utility