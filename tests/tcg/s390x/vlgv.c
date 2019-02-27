/*
 * QEMU TCG Tests - s390x VECTOR LOAD GR FROM VR ELEMENT
 *
 * Copyright (C) 2019 Red Hat Inc
 *
 * Authors:
 *   David Hildenbrand <david@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
#include <stdint.h>
#include <unistd.h>
#include "signal-helper.inc.c"

static inline void vlgv(uint64_t *r1, S390Vector *v3, uint64_t b2, uint8_t m4)
{
    asm volatile("vlgv %[r1], %[v3], 0(%[b2]), %[m4]\n"
                 : [r1] "+d" (*r1),
                   [v3] "+v" (v3->v)
                 : [b2] "d" (b2),
                   [m4] "i" (m4));
}

static inline void vlgv_nob2(uint64_t *r1, S390Vector *v3, uint16_t d2,
                             uint8_t m4)
{
    asm volatile("vlgv %[r1], %[v3], %[d2](0), %[m4]\n"
                 : [r1] "+d" (*r1),
                   [v3] "+v" (v3->v)
                 : [d2] "i" (d2),
                   [m4] "i" (m4));
}

int main(void)
{
    S390Vector v3 = {
        .d[0] = 0x0011223344556677ull,
        .d[1] = 0x8899aabbccddeeffull,
    };
    uint64_t r1 = 0;

    /* Use d2 only - set all unused bits to 1 to test if they will be ignored */
    vlgv_nob2(&r1, &v3, 7 | 0xff0u, ES_8);
    check("8 bit", r1 == 0x77);
    vlgv_nob2(&r1, &v3, 4 | 0xff8u, ES_16);
    check("16 bit", r1 == 0x8899);
    vlgv_nob2(&r1, &v3, 3 | 0xffcu, ES_32);
    check("32 bit", r1 == 0xccddeeff);
    vlgv_nob2(&r1, &v3, 1 | 0xffeu, ES_64);
    check("64 bit", r1 == 0x8899aabbccddeeffull);

    /* Use b2 - set all unused bits to 1 to test if they will be ignored */
    vlgv(&r1, &v3, 7 | ~0xfull, ES_8);
    check("8 bit", r1 == 0x77);
    vlgv(&r1, &v3, 4 | ~0x7ull, ES_16);
    check("16 bit", r1 == 0x8899);
    vlgv(&r1, &v3, 3 | ~0x3ull, ES_32);
    check("32 bit", r1 == 0xccddeeff);
    vlgv(&r1, &v3, 1 | ~0x1ull, ES_64);
    check("64 bit", r1 == 0x8899aabbccddeeffull);

    check("v3 not modified", v3.d[0] == 0x0011223344556677ull &&
                             v3.d[1] == 0x8899aabbccddeeffull);

    CHECK_SIGILL(vlgv(&r1, &v3, 0, ES_128));
    return 0;
}
