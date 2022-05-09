#pragma once
namespace utility {
class KeyToggle {
   public:
    explicit KeyToggle(int vKey) : vKey(vKey) {}
    explicit KeyToggle(char key);
    explicit operator bool();
    // the key was switched on/off by pressing it
    bool isToggled();
    char getChar();

   private:
    // prevents toggle while holding the key
    bool state = false;
    bool toggled = false;
    short vKey;
};
}  // namespace utility
