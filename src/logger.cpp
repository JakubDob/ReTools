#include "logger.h"

#include <ctime>
#include <sstream>

namespace utility {
std::string hex(std::string data) {
    std::stringstream dbg;
    for (size_t i = 0; i < data.length(); i++) {
        int byte = static_cast<int>((*reinterpret_cast<byte_t*>(&data[i])));
        if (byte <= 0xf) {
            dbg << "\\x0" << std::hex << byte;
        } else {
            dbg << "\\x" << std::hex << byte;
        }
    }
    return dbg.str();
}

}  // namespace utility