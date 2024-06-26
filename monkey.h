#pragma once

#ifdef __cplusplus
    #define N(n) n

    #include <cassert>
    #include <cstring>
    #include <functional>
    #include <type_traits>

#else // __cplusplus
    #define N(n) monkeyc_##n

    #include <assert.h>
    #include <string.h>
#endif // __cplusplus

#ifdef _WIN32
    #include <Windows.h>
#elif __unix__
    #include <sys/mman.h>
    #include <unistd.h>
#else
    #error "Unsupported platform"
#endif


#if defined(__x86_64__) || defined(_M_X64)
    #define MONKEYC_ENV64BIT
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    #define MONKEYC_ENV32BIT
#else
    #error "Unsupported architecture"
#endif

#ifdef __cplusplus
namespace monkeyc {
#endif

    typedef unsigned char N(u8);

#define u8 N(u8)

#ifdef _WIN32

    static void N(copy_to_addr_unsafe)(void *dst, void *src, size_t len) {
        DWORD old_perms = 0;
        BOOL success = VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &old_perms);
        assert(success);
        memcpy(dst, src, len);
        DWORD temp = 0;
        success = VirtualProtect(dst, len, old_perms, &temp);
        assert(success);
    }

#else

static void N(copy_to_addr_unsafe)(void *dst, void *src, size_t len) {
    size_t page_size = sysconf(_SC_PAGESIZE);
    void *page_start = (void *) ((size_t) dst & ~(page_size - 1));
    int res = mprotect(page_start, page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
    assert(res == 0);
    memcpy(dst, src, len);
    res = mprotect(page_start, page_size, PROT_READ | PROT_EXEC);
    assert(res == 0);
}

#endif

#ifdef MONKEYC_ENV64BIT

    #ifdef __cplusplus
    const size_t MONKEYC_JUMP_SIZE = 12;
    #else
        #define MONKEYC_JUMP_SIZE 12
    #endif

    static inline void N(assemble_jmp_instr)(void *dst, u8 instr_buf[MONKEYC_JUMP_SIZE]) {
        size_t addr = (size_t) dst;
        // movabs rdx, dst
        instr_buf[0] = 0x48;
        instr_buf[1] = 0xBA;
        instr_buf[2] = (u8) addr;
        instr_buf[3] = (u8) (addr >> 8);
        instr_buf[4] = (u8) (addr >> 16);
        instr_buf[5] = (u8) (addr >> 24);
        instr_buf[6] = (u8) (addr >> 32);
        instr_buf[7] = (u8) (addr >> 40);
        instr_buf[8] = (u8) (addr >> 48);
        instr_buf[9] = (u8) (addr >> 56);
        // jmp rdx
        instr_buf[10] = 0xFF;
        instr_buf[11] = 0xE2;
    }

#elif MONKEYC_ENV32BIT

    #ifdef __cplusplus
const size_t MONKEYC_JUMP_SIZE = 7;
    #else
        #define MONKEYC_JUMP_SIZE 7
    #endif

static inline void N(assemble_jmp_instr)(void *dst, u8 instr_buf[MONKEYC_JUMP_SIZE]) {
    size_t addr = (size_t) dst;
    // movabs edx, dst
    instr_buf[0] = 0xBA;
    instr_buf[1] = (u8) addr;
    instr_buf[2] = (u8) (addr >> 8);
    instr_buf[3] = (u8) (addr >> 16);
    instr_buf[4] = (u8) (addr >> 24);
    // jmp edx
    instr_buf[5] = 0xFF;
    instr_buf[6] = 0xE2;
}

#endif

#ifndef __cplusplus

    typedef struct {
        void *ptr;
        u8 instr[MONKEYC_JUMP_SIZE];
    } monkeyc_patched;

    static monkeyc_patched monkeyc_patch(void *target_f, void *replacement_f) {
        monkeyc_patched p;
        u8 patch[MONKEYC_JUMP_SIZE];
        N(assemble_jmp_instr)
        (replacement_f, patch);
        memcpy(p.instr, target_f, MONKEYC_JUMP_SIZE);
        N(copy_to_addr_unsafe)
        (target_f, patch, MONKEYC_JUMP_SIZE);
        p.ptr = target_f;
        return p;
    }

    static void monkeyc_unpatch(monkeyc_patched *p) {
        N(copy_to_addr_unsafe)
        (p->ptr, p->instr, MONKEYC_JUMP_SIZE);
    }

#else  // __cplusplus

class PatchGuard {
public:
    void *ptr;
    u8 instr[MONKEYC_JUMP_SIZE];

    inline ~PatchGuard() {
        N(copy_to_addr_unsafe)
        (this->ptr, this->instr, MONKEYC_JUMP_SIZE);
    };
};

template<typename F>
inline constexpr bool is_valid_function_type = std::is_member_function_pointer_v<F>;

template<typename F>
inline constexpr bool is_valid_function_type<F *> = std::is_function_v<F>;

template<typename F>
inline constexpr bool is_valid_patch_type = std::is_class_v<F> || is_valid_function_type<F>;

template<typename F, class = void>
union UnsafeCaster {
    F f;
    void *p{};

    inline explicit UnsafeCaster(F f) {
        this->f = f;
    }
};

// special handling for lambdas
template<typename F>
union UnsafeCaster<F, typename std::enable_if_t<std::is_class_v<F>>> {
    void *f;
    void *p{};

    inline explicit UnsafeCaster(F f) {
        static_assert(is_valid_function_type<decltype(+f)>, "+f must be a function if passing in an object");
        this->f = (void *) (+f);
    }
};

template<typename FT, typename FR>
static PatchGuard patch(FT target, FR replacement) {
    static_assert(is_valid_patch_type<FT> && is_valid_patch_type<FR>, "must be function type");
    PatchGuard g{};
    UnsafeCaster<FT> uT(target);
    UnsafeCaster<FR> uR(replacement);
    u8 patch[MONKEYC_JUMP_SIZE];
    N(assemble_jmp_instr)
    (uR.p, patch);
    memcpy(g.instr, uT.p, MONKEYC_JUMP_SIZE);
    N(copy_to_addr_unsafe)
    (uT.p, patch, MONKEYC_JUMP_SIZE);
    g.ptr = uT.p;
    return g;
}
}
#endif // __cplusplus

#undef N
#undef u8
