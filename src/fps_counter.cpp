#include "fps_counter.h"

namespace utility {
bool FPScounter::tickAndCanReturn() {
    frameCount++;
    if (!initialized) {
        t1 = std::chrono::high_resolution_clock::now();
        initialized = true;
        return false;
    }
    t2 = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    if (duration >= 1 && frameCount > 5) {
        t1 = t2;
        str = std::to_string(frameCount / duration) + " FPS";
        frameCount = 0;
        return true;
    }
    return false;
}
std::string FPScounter::getString() { return str; }
}  // namespace utility