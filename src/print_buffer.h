#pragma once
#include "console.h"

namespace utility {
void printBuffer(Console &c, char *buffer, size_t len);
void saveBuffer(std::string const &path, byte_t *buffer, size_t len);
}  // namespace utility
