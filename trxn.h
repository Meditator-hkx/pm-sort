#ifndef PM_BASIC_TRXN_H
#define PM_BASIC_TRXN_H

#include <iostream>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <stdint.h>
#include <string.h>
// #include <stdatomic.h>

inline void flush(char *addr) {
    _mm_clflush(addr);
    // _mm_clwb(addr);
}

inline void fence() {
    _mm_mfence();
    // _mm_sfence();
}

inline void flush_range(char *addr, uint32_t size) {
    int round = (size-1) / 64 + 1;
    for (int i = 0;i < round;i++) {
        flush(addr);
        addr += 64;
    }
}

inline void clear_cache(char *addr, int size) {
    int round = (size-1) / 64 + 1;
    for (int i = 0;i < round;i++) {
        _mm_clflush(addr);
        addr += 64;
    }
}

inline void ntstore(char *addr, char *wbuffer, uint32_t size) {
    int *s = (int *)addr;
    int *t = (int *)wbuffer;

    int round = (size-1) / 4 + 1;
    for (int i = 0;i < round;i++) {
        _mm_stream_si32(s, *t);
        s++;
        t++;
    }
}


// mem: the target address to be modified
// with: the target number to replace the original value in mem
// cmp: the compared value
//inline uint64_t CAS(volatile uint64_t *mem, uint64_t with, uint64_t cmp) {
//    uint64_t prev = cmp;
//    // This version by Mans Rullgard of Pathscale
//    __asm__ __volatile__ ( "lock\n\t"
//                           "cmpxchg %2,%0"
//    : "+m"(*mem), "+a"(prev)
//    : "r"(with)
//    : "cc");
//
//    return prev;
//}

//inline uint64_t FAA(volatile uint64_t *mem, uint64_t operand) {
//    return CAS(mem, *mem + operand, *mem);
//}

#endif //PM_BASIC_TRXN_H
