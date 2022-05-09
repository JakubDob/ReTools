#include "print_buffer.h"

namespace utility {
void printBuffer(Console &c, char *buffer, size_t len) {
    if (len == 0) return;
    c << "(" << static_cast<int>(len) << ") ";
    (len > 9 && len < 100) ? c << " " : c << "  ";
    for (unsigned i = 0; i < len; i++) {
        c << intToHexStr(static_cast<byte_t>(buffer[i])) << " ";
    }
    c << "\n";
}
void saveBuffer(std::string const &path, byte_t *buffer, size_t len) {
    if (len == 0) return;
    std::ofstream file(path, std::ios::app);
    file << "(" << static_cast<int>(len) << ") ";
    if (len < 9)
        file << "    ";
    else if (len < 100)
        file << "   ";
    else if (len < 1000)
        file << "  ";
    else
        file << " ";
    for (unsigned i = 0; i < len; i++) {
        file << intToHexStr(buffer[i]) << " ";
    }
    file << "\n";
}
}  // namespace utility