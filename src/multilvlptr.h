#pragma once
#include <vector>

#include "types.h"

namespace utility {
template <typename T>
class MultiLvlPtr {
   public:
    MultiLvlPtr(std::vector<long long> offsets) {
        if (!offsets.empty()) {
            addr_t x = 0;
            for (size_t i = 0; i < offsets.size() - 1; i++) {
                x = *reinterpret_cast<addr_t*>(x + offsets[i]);
            }
            value = reinterpret_cast<T*>(x + offsets.back());
        }
    }
    T* operator->() { return value; }
    T& operator*() { return *value; }
    T& operator=(T const& other) { value = other; }
    T* get() { return value; }
    T* operator++(int) { return value++; }

   private:
    T* value;
};
}  // namespace utility
