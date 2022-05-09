#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "key_toggle.h"

#include <Windows.h>

namespace utility {
KeyToggle::KeyToggle(char key) {
    HKL currentKBL = GetKeyboardLayout(0);
    vKey = VkKeyScanExA(key, currentKBL) & 0xFF;
}
KeyToggle::operator bool() {
    if (GetAsyncKeyState(vKey)) {
        if (!state) {
            state = true;
            toggled = !toggled;
            return true;
        }
    } else {
        state = false;
    }
    return false;
}
bool KeyToggle::isToggled() { return toggled; }
char KeyToggle::getChar() {
    char key = MapVirtualKeyA(vKey, MAPVK_VK_TO_CHAR);
    return key;
}
}  // namespace utility
