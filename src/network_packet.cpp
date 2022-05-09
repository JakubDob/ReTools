#include "network_packet.h"

namespace utility {
namespace network {
void swapEndianness(char* buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (i >= (size - 1 - i)) break;
        uint8_t tmp = buf[i];
        buf[i] = buf[size - 1 - i];
        buf[size - 1 - i] = tmp;
    }
}
char* extractData(std::string& s) { return s.data(); }
size_t extractSize(std::string s) { return s.size(); }
}  // namespace network
}  // namespace utility