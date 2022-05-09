#pragma once
#include <array>
#include <string>
#include <vector>

/*
Class used for interpreting arbitrary network data, eg:
input: 0x1, 0x2, 0x3, 0x4
NetworkPacket<uint8_t, uint16_t, uint8_t> will store the first byte as uint8_t,
second and third as uint16_t and fourth as uint8_t stored values can be modified
in a tuple
*/
namespace utility {
namespace network {
template <typename T>
struct type_t {};
template <typename T>
constexpr auto type = type_t<T>{};
template <std::size_t N>
struct unknown_t {
    uint8_t data[N];
};

char *extractData(std::string &s);
size_t extractSize(std::string s);
void swapEndianness(char *buf, size_t size);

template <typename = void, typename... Args>
struct test : std::false_type {};

template <typename... Args>
struct test<std::void_t<decltype(extractData(std::declval<Args>()...))>,
            Args...> : std::true_type {};

template <typename... Args>
inline constexpr bool test_v = test<void, Args...>::value;

template <typename T>
void swapEndianness(T &t) {
    if constexpr (sizeof(T) <= 64) {
        size_t sz = sizeof(T);
        uint8_t bytes[64];
        memcpy(&bytes, &t, sz);
        swapEndianness((char *)bytes, sz);
        memcpy(&t, &bytes, sz);
    }
}

template <typename T>
std::tuple<T, size_t> fromBytes(type_t<T>, char *data) {
    return std::make_tuple(*reinterpret_cast<T *>(data), sizeof(T));
}
template <typename T>
auto toBytes(T data) {
    std::vector<uint8_t> vec(sizeof(T));
    memcpy(vec.data(), &data, sizeof(T));
    return vec;
}

class BaseNetworkPacket {
   public:
    static void setSwapEndianness(bool shouldSwap) {
        shouldSwapEndian = shouldSwap;
    }

   protected:
    BaseNetworkPacket(BaseNetworkPacket &other) = delete;
    BaseNetworkPacket(BaseNetworkPacket &&) = delete;
    BaseNetworkPacket &operator=(BaseNetworkPacket const &other) = delete;
    BaseNetworkPacket &operator=(BaseNetworkPacket &&other) = delete;
    ~BaseNetworkPacket() {}

    BaseNetworkPacket(){};
    static inline bool shouldSwapEndian = false;

   private:
    inline static std::vector<BaseNetworkPacket *> children =
        std::vector<BaseNetworkPacket *>();
};

template <typename... Args>
class NetworkPacket : BaseNetworkPacket {
   public:
    NetworkPacket() : staticBufferSize(getArgsByteSize()) {}
    void serialize() {
        serializeOffset = 0;
        std::apply(
            [&](auto &&...args) { (serializeHelper(toBytes(args)), ...); },
            m_outputTuple);
    }
    void deserialize(char *buf) {
        deserializeOffset = 0;
        inputByteBuffer = buf;
        std::apply([&](auto &&...args) { (deserializeHelper(args), ...); },
                   m_inputTuple);
    }
    void serializeInput() {
        serializeInputOffset = 0;
        std::apply(
            [&](auto &&...args) { (serializeInputHelper(toBytes(args)), ...); },
            m_inputTuple);
    }
    auto &&inputTuple() { return m_inputTuple; }
    auto &&outputTuple() { return m_outputTuple; }

    uint8_t *outputBuffer() { return outputByteBuffer.data(); }
    size_t getDynamicBufferSize() const { return outputByteBuffer.size(); }
    size_t getStaticBufferSize() const { return staticBufferSize; }

   private:
    std::tuple<Args...> m_inputTuple;
    std::tuple<Args...> m_outputTuple;
    size_t deserializeOffset = 0;
    size_t serializeOffset = 0;
    size_t serializeInputOffset = 0;
    size_t staticBufferSize = 0;

    char *inputByteBuffer = nullptr;
    std::vector<uint8_t> outputByteBuffer;
    template <std::size_t N, typename T>
    void serializeHelper(std::array<T, N> &&arr) {
        if (shouldSwapEndian) {
            swapEndianness(arr);
        }
        if (outputByteBuffer.size() < serializeOffset + N) {
            outputByteBuffer.resize(serializeOffset + N);
        }
        std::copy(arr.begin(), arr.end(),
                  outputByteBuffer.data() + serializeOffset);
        serializeOffset += N;
    }
    template <typename T>
    void serializeHelper(std::vector<T> &&vec) {
        if (shouldSwapEndian) {
            swapEndianness((char *)vec.data(), vec.size());
        }
        if (outputByteBuffer.size() < serializeOffset + vec.size()) {
            outputByteBuffer.resize(serializeOffset + vec.size());
        }
        std::copy(vec.begin(), vec.end(),
                  outputByteBuffer.data() + serializeOffset);
        serializeOffset += vec.size();
    }
    template <std::size_t N, typename T>
    void serializeInputHelper(std::array<T, N> &&arr) {
        if (shouldSwapEndian) {
            swapEndianness(arr);
        }
        std::copy(arr.begin(), arr.end(),
                  inputByteBuffer + serializeInputOffset);
        serializeInputOffset += N;
    }
    template <typename T>
    void serializeInputHelper(std::vector<T> &&vec) {
        if (shouldSwapEndian) {
            swapEndianness((char *)vec.data(), vec.size());
        }
        std::copy(vec.begin(), vec.end(),
                  inputByteBuffer + serializeInputOffset);
        serializeInputOffset += vec.size();
    }

    template <typename R>
    void deserializeHelper(R &&refToTupleElem) {
        auto t = fromBytes(type<std::remove_reference_t<R>>,
                           inputByteBuffer + deserializeOffset);
        if (shouldSwapEndian) {
            auto item = std::get<0>(t);
            if constexpr (test_v<R>) {
                swapEndianness(extractData(item), extractSize(item));
            } else {
                swapEndianness(item);
            }
            refToTupleElem = item;
        } else {
            refToTupleElem = std::get<0>(t);
        }
        deserializeOffset += std::get<1>(t);
    }

    template <typename T, typename... A>
    size_t getArgsByteSizeImpl(int i) {
        if constexpr (sizeof...(A) == 0) {
            return i + sizeof(T);
        } else {
            return getArgsByteSizeImpl<A...>(i + sizeof(T));
        }
    }

    size_t getArgsByteSize() { return getArgsByteSizeImpl<Args...>(0); }
};
}  // namespace network
}  // namespace utility