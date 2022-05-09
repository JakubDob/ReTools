#pragma once
#include <cstdint>
#include <cstring>

namespace utility {
namespace AsmEmu {
union Reg {
    int64_t R;
    int32_t E;
    int16_t X;
    struct {
        int8_t L;
        int8_t H;
    };
};

struct RAX {
    static char const id = 0;
    static int64_t& get(Reg* const reg) { return reg[0].R; }
};
struct EAX {
    static int32_t& get(Reg* const reg) { return reg[0].E; }
};
struct AX {
    static int16_t& get(Reg* const reg) { return reg[0].X; }
};
struct AL {
    static int8_t& get(Reg* const reg) { return reg[0].L; }
};
struct AH {
    static int8_t& get(Reg* const reg) { return reg[0].H; }
};

struct RBX {
    static char const id = 1;
    static int64_t& get(Reg* const reg) { return reg[1].R; }
};
struct EBX {
    static int32_t& get(Reg* const reg) { return reg[1].E; }
};
struct BX {
    static int16_t& get(Reg* const reg) { return reg[1].X; }
};
struct BL {
    static int8_t& get(Reg* const reg) { return reg[1].L; }
};
struct BH {
    static int8_t& get(Reg* const reg) { return reg[1].H; }
};

struct RCX {
    static char const id = 2;
    static int64_t& get(Reg* const reg) { return reg[2].R; }
};
struct ECX {
    static int32_t& get(Reg* const reg) { return reg[2].E; }
};
struct CX {
    static int16_t& get(Reg* const reg) { return reg[2].X; }
};
struct CL {
    static int8_t& get(Reg* const reg) { return reg[2].L; }
};
struct CH {
    static int8_t& get(Reg* const reg) { return reg[2].H; }
};

struct RDX {
    static char const id = 3;
    static int64_t& get(Reg* const reg) { return reg[3].R; }
};
struct EDX {
    static int32_t& get(Reg* const reg) { return reg[3].E; }
};
struct DX {
    static int16_t& get(Reg* const reg) { return reg[3].X; }
};
struct DL {
    static int8_t& get(Reg* const reg) { return reg[3].L; }
};
struct DH {
    static int8_t& get(Reg* const reg) { return reg[3].H; }
};

struct RSP {
    static char const id = 4;
    static int64_t& get(Reg* const reg) { return reg[4].R; }
};
struct ESP {
    static int32_t& get(Reg* const reg) { return reg[4].E; }
};
struct SP {
    static int16_t& get(Reg* const reg) { return reg[4].X; }
};
struct SPL {
    static int8_t& get(Reg* const reg) { return reg[4].L; }
};

struct RBP {
    static char const id = 5;
    static int64_t& get(Reg* const reg) { return reg[5].R; }
};
struct EBP {
    static int32_t& get(Reg* const reg) { return reg[5].E; }
};
struct BP {
    static int16_t& get(Reg* const reg) { return reg[5].X; }
};
struct BPL {
    static int8_t& get(Reg* const reg) { return reg[5].L; }
};

struct RSI {
    static char const id = 6;
    static int64_t& get(Reg* const reg) { return reg[6].R; }
};
struct ESI {
    static int32_t& get(Reg* const reg) { return reg[6].E; }
};
struct SI {
    static int16_t& get(Reg* const reg) { return reg[6].X; }
};
struct SIL {
    static int8_t& get(Reg* const reg) { return reg[6].L; }
};

struct RDI {
    static char const id = 7;
    static int64_t& get(Reg* const reg) { return reg[7].R; }
};
struct EDI {
    static int32_t& get(Reg* const reg) { return reg[7].E; }
};
struct DI {
    static int16_t& get(Reg* const reg) { return reg[7].X; }
};
struct DIL {
    static int8_t& get(Reg* const reg) { return reg[7].L; }
};

class AsmEmulator_x86 {
   public:
    AsmEmulator_x86() { clear(); };
    void clear() { memset(regs, 0, 8 * sizeof(uint64_t)); }
    template <typename D, typename S>
    void mov(S s) {
        D::get(regs) = s & mask[sizeof(S)];
    }
    template <typename D, typename S>
    void mov() {
        D::get(regs) = S::get(regs);
    };
    template <typename D>
    void inc() {
        D::get(regs)++;
    }
    template <typename D>
    void dec() {
        D::get(regs)--;
    }
    template <typename D, typename S>
    void add(S s) {
        D::get(regs) += s & mask[sizeof(S)];
    }
    template <typename D, typename S>
    void add() {
        D::get(regs) += S::get(regs);
    }
    template <typename D, typename S>
    void sub(S s) {
        D::get(regs) -= s & mask[sizeof(S)];
    }
    template <typename D, typename S>
    void sub() {
        D::get(regs) -= S::get(regs);
    }
    template <typename D, typename S>
    void lea(S s) {
        D::get(regs) = s & mask[sizeof(S)];
    }
    template <typename D>
    void shl(int count) {
        D::get(regs) <<= count;
    }
    template <typename D, typename S>
    void shl() {
        D::get(regs) <<= S::get(regs);
    }
    template <typename D>
    void shr(int count) {
        D::get(regs) >>= count;
    }
    template <typename D, typename S>
    void shr() {
        D::get(regs) >>= S::get(regs);
    }
    template <typename D, typename S>
    void and_(S s) {
        D::get(regs) &= (s & mask[sizeof(S)]);
    }
    template <typename D, typename S>
    void and_() {
        D::get(regs) &= S::get(regs);
    }
    template <typename D, typename S>
    void or_(S s) {
        D::get(regs) |= (s & mask[sizeof(S)]);
    }
    template <typename D, typename S>
    void or_() {
        D::get(regs) |= S::get(regs);
    }
    template <typename D, typename S>
    void movsx(S s) {
        D::get(regs) = s & mask[sizeof(S)];
        if (s < 0) D::get(regs) &= mask2[sizeof(S)];
    }

    template <typename R>
    int64_t getRegVal() {
        return regs[R::id].R;
    }

    int64_t rax() { return regs[0].R; };
    int64_t rbx() { return regs[1].R; };
    int64_t rcx() { return regs[2].R; };
    int64_t rdx() { return regs[3].R; };
    int64_t rsp() { return regs[4].R; };
    int64_t rbp() { return regs[5].R; };
    int64_t rsi() { return regs[6].R; };
    int64_t rdi() { return regs[7].R; };

    int32_t eax() { return regs[0].E; };
    int32_t ebx() { return regs[1].E; };
    int32_t ecx() { return regs[2].E; };
    int32_t edx() { return regs[3].E; };
    int32_t esp() { return regs[4].E; };
    int32_t ebp() { return regs[5].E; };
    int32_t esi() { return regs[6].E; };
    int32_t edi() { return regs[7].E; };

    int16_t ax() { return regs[0].X; };
    int16_t bx() { return regs[1].X; };
    int16_t cx() { return regs[2].X; };
    int16_t dx() { return regs[3].X; };
    int16_t sp() { return regs[4].X; };
    int16_t bp() { return regs[5].X; };
    int16_t si() { return regs[6].X; };
    int16_t di() { return regs[7].X; };

    int8_t ah() { return regs[0].H; };
    int8_t al() { return regs[0].L; };
    int8_t bh() { return regs[1].H; };
    int8_t bl() { return regs[1].L; };
    int8_t ch() { return regs[2].H; };
    int8_t cl() { return regs[2].L; };
    int8_t dh() { return regs[3].H; };
    int8_t dl() { return regs[3].L; };
    int8_t spl() { return regs[4].L; };
    int8_t bpl() { return regs[5].L; };
    int8_t sil() { return regs[6].L; };
    int8_t dil() { return regs[7].L; };

   protected:
   private:
    Reg regs[8];
    uint64_t mask[9] = {0,
                        0xFF,
                        0xFFFF,
                        0xFFFFFF,
                        0xFFFFFFFF,
                        0xFFFFFFFFFF,
                        0xFFFFFFFFFFFF,
                        0xFFFFFFFFFFFFFF,
                        0xFFFFFFFFFFFFFFFF};
    uint64_t mask2[9] = {
        0xFFFFFFFFFFFFFFFF, 0x00FFFFFFFFFFFFFF, 0x0000FFFFFFFFFFFF,
        0x000000FFFFFFFFFF, 0x00000000FFFFFFFF, 0x0000000000FFFFFF,
        0x000000000000FFFF, 0x00000000000000FF, 0};
};
}  // namespace AsmEmu
}  // namespace utility