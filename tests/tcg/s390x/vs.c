/*
 * QEMU TCG Tests - s390x VECTOR SUBTRACT *
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

static inline void vs(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                      uint8_t m4)
{
    asm volatile("vs %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline void vscbi(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                         uint8_t m4)
{
    asm volatile("vscbi %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline void vsbi(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        S390Vector *v4, uint8_t m5)
{
    asm volatile("vsbi %[v1], %[v2], %[v3], %[v4], %[m5]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v),
                   [v4] "+v" (v4->v)
                 : [m5] "i" (m5));
}

static inline void vsbcbi(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                          S390Vector *v4, uint8_t m5)
{
    asm volatile("vsbcbi %[v1], %[v2], %[v3], %[v4], %[m5]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v),
                   [v4] "+v" (v4->v)
                 : [m5] "i" (m5));
}

int main(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0x0011223344556677ull,
        .d[1] = 0x8899aabbccddeeffull,
    };
    S390Vector v3 = {
        .d[0] = 0xffeeddccbbaa9988ull,
        .d[1] = 0x7766554433221100ull,
    };
    S390Vector v4 = {
        .d[0] = 0xffeeddccbbaa9988ull,
        .d[1] = 0x8899aabbccddeeffull,
    };

    /* v1 = v2 - v3 */
    vs(&v1, &v2, &v3, ES_8);
    check("vs: 8", v1.d[0] == 0x0123456789abcdefull &&
                   v1.d[1] == 0x1133557799bbddffull);
    vs(&v1, &v2, &v3, ES_16);
    check("vs: 16", v1.d[0] == 0x0023446788abccefull &&
                    v1.d[1] == 0x1133557799bbddffull);
    vs(&v1, &v2, &v3, ES_32);
    check("vs: 32", v1.d[0] == 0x0022446788aaccefull &&
                    v1.d[1] == 0x1133557799bbddffull);
    vs(&v1, &v2, &v3, ES_64);
    check("vs: 64", v1.d[0] == 0x0022446688aaccefull &&
                    v1.d[1] == 0x1133557799bbddffull);
    vs(&v1, &v2, &v3, ES_128);
    check("vs: 128", v1.d[0] == 0x0022446688aaccefull &&
                     v1.d[1] == 0x1133557799bbddffull);

    /* v1 = v3 - v2 */
    vs(&v1, &v3, &v2, ES_8);
    check("vs: 8", v1.d[0] == 0xffddbb9977553311ull &&
                   v1.d[1] == 0xefcdab8967452301ull);
    vs(&v1, &v3, &v2, ES_16);
    check("vs: 16", v1.d[0] == 0xffddbb9977553311ull &&
                    v1.d[1] == 0xeecdaa8966452201ull);
    vs(&v1, &v3, &v2, ES_32);
    check("vs: 32", v1.d[0] == 0xffddbb9977553311ull &&
                    v1.d[1] == 0xeeccaa8966442201ull);
    vs(&v1, &v3, &v2, ES_64);
    check("vs: 64", v1.d[0] == 0xffddbb9977553311ull &&
                    v1.d[1] == 0xeeccaa8866442201ull);
    vs(&v1, &v3, &v2, ES_128);
    check("vs: 128", v1.d[0] == 0xffddbb9977553310ull &&
                     v1.d[1] == 0xeeccaa8866442201ull);

    /* v1 contains the borrow if v3 - v2 (weird parameter exchange) */
    vscbi(&v1, &v2, &v3, ES_8);
    check("vscbi: 8", !v1.d[0] && v1.d[1] == 0x0101010101010101ull);
    vscbi(&v1, &v2, &v3, ES_16);
    check("vscbi: 16", !v1.d[0] && v1.d[1] == 0x0001000100010001ull);
    vscbi(&v1, &v2, &v3, ES_32);
    check("vscbi: 32", !v1.d[0] && v1.d[1] == 0x0000000100000001ull);
    vscbi(&v1, &v2, &v3, ES_64);
    check("vscbi: 64", !v1.d[0] && v1.d[1] == 0x0000000000000001ull);
    vscbi(&v1, &v2, &v3, ES_128);
    check("vscbi: 128", !v1.d[0] && !v1.d[1]);

    /* v1 contains the borrow of v2 - v3 (weird parameter exchange) */
    vscbi(&v1, &v3, &v2, ES_8);
    check("vscbi: 8", v1.d[0] == 0x0101010101010101ull && !v1.d[1]);
    vscbi(&v1, &v3, &v2, ES_16);
    check("vscbi: 16", v1.d[0] == 0x0001000100010001ull && !v1.d[1]);
    vscbi(&v1, &v3, &v2, ES_32);
    check("vscbi: 32", v1.d[0] == 0x0000000100000001ull && !v1.d[1]);
    vscbi(&v1, &v3, &v2, ES_64);
    check("vscbi: 64", v1.d[0] == 0x0000000000000001ull && !v1.d[1]);
    vscbi(&v1, &v3, &v2, ES_128);
    check("vscbi: 128", !v1.d[0] && v1.d[1] == 0x0000000000000001ull);

    /* Do different subtractions with different borrows */
    vsbi(&v1, &v2, &v3, &v4, ES_128);
    check("vsbi", v1.d[0] == 0x0022446688aaccefull &&
                  v1.d[1] == 0x1133557799bbddffull);
    vsbi(&v1, &v3, &v2, &v4, ES_128);
    check("vsbi", v1.d[0] == 0xffddbb9977553310ull &&
                  v1.d[1] == 0xeeccaa8866442201ull);
    vsbi(&v1, &v3, &v2, &v3, ES_128);
    check("vsbi", v1.d[0] == 0xffddbb9977553310ll &&
                  v1.d[1] == 0xeeccaa8866442200ull);

    /* Do different subtractions with different borrows */
    vsbcbi(&v1, &v2, &v3, &v4, ES_128);
    check("vsbcbi", !v1.d[0] && !v1.d[1]);
    vsbcbi(&v1, &v3, &v2, &v4, ES_128);
    check("vsbcbi", !v1.d[0] && v1.d[1] == 0x0000000000000001ull);
    vsbcbi(&v1, &v3, &v2, &v3, ES_128);
    check("vsbcbi", !v1.d[0] && v1.d[1] == 0x0000000000000001ull);

    /* 0-0 (no borrow) */
    v2.d[0] = v2.d[1] = 0;
    vs(&v1, &v2, &v2, ES_8);
    check("vs(0-0): 8", !v1.d[0] && !v1.d[1]);
    vs(&v1, &v2, &v2, ES_16);
    check("vs(0-0): 16", !v1.d[0] && !v1.d[1]);
    vs(&v1, &v2, &v2, ES_32);
    check("vs(0-0): 32", !v1.d[0] && !v1.d[1]);
    vs(&v1, &v2, &v2, ES_64);
    check("vs(0-0): 64", !v1.d[0] && !v1.d[1]);
    vs(&v1, &v2, &v2, ES_128);
    check("vs(0-0): 128", !v1.d[0] && !v1.d[1]);
    vscbi(&v1, &v2, &v2, ES_8);
    check("vscbi(0-0): 8", v1.d[0] == 0x0101010101010101ull &&
                           v1.d[1] == 0x0101010101010101ull);
    vscbi(&v1, &v2, &v2, ES_16);
    check("vscbi(0-0): 16", v1.d[0] == 0x0001000100010001ull &&
                            v1.d[1] == 0x0001000100010001ull);
    vscbi(&v1, &v2, &v2, ES_32);
    check("vscbi(0-0): 32", v1.d[0] == 0x0000000100000001ull &&
                            v1.d[1] == 0x0000000100000001ull);
    vscbi(&v1, &v2, &v2, ES_64);
    check("vscbi(0-0): 64", v1.d[0] == 1 && v1.d[1] == 1);
    vscbi(&v1, &v2, &v2, ES_128);
    check("vscbi(0-0): 128", !v1.d[0] && v1.d[1] == 0x0000000000000001ull);
    vsbi(&v1, &v2, &v2, &v2, ES_128);
    check("vsbi(0-0)", v1.d[0] == -1ull && v1.d[1] == -1ull);
    vsbcbi(&v1, &v2, &v2, &v2, ES_128);
    check("vsbcbi(0-0)", !v1.d[0] && !v1.d[1]);

    /* 0-0 (borrow) */
    v4.d[0] = v4.d[1] = -1ull;
    vsbi(&v1, &v2, &v2, &v4, ES_128);
    check("vsbi(0-0) - borrow", !v1.d[0] && !v1.d[1]);
    vsbcbi(&v1, &v2, &v2, &v4, ES_128);
    check("vsbcbi(0-0) - borrow", !v1.d[0] && v1.d[1] == 1);

    /* 0-1 (no borrow) */
    v3.d[0] = v3.d[1] = 1;
    vs(&v1, &v2, &v3, ES_8);
    check("vs(0-1): 8", v1.d[0] == 0x00000000000000ffull &&
                        v1.d[1] == 0x00000000000000ffull);
    vs(&v1, &v2, &v3, ES_16);
    check("vs(0-1): 16", v1.d[0] == 0x000000000000ffffull &&
                         v1.d[1] == 0x000000000000ffffull);
    vs(&v1, &v2, &v3, ES_32);
    check("vs(0-1): 32", v1.d[0] == 0x00000000ffffffffull &&
                         v1.d[1] == 0x00000000ffffffffull);
    vs(&v1, &v2, &v3, ES_64);
    check("vs(0-1): 64", v1.d[0] == 0xffffffffffffffffull &&
                         v1.d[1] == 0xffffffffffffffffull);
    vs(&v1, &v2, &v3, ES_128);
    check("vs(0-1)", v1.d[0] == 0xfffffffffffffffeull &&
                     v1.d[1] == 0xffffffffffffffffull);
    vscbi(&v1, &v2, &v3, ES_8);
    check("vscbi(0-1): 8", v1.d[0] == 0x0101010101010100ull &&
                           v1.d[1] == 0x0101010101010100ull);
    vscbi(&v1, &v2, &v3, ES_16);
    check("vscbi(0-1): 16", v1.d[0] == 0x0001000100010000ull &&
                            v1.d[1] == 0x0001000100010000ull);
    vscbi(&v1, &v2, &v3, ES_32);
    check("vscbi(0-1): 32", v1.d[0] == 0x0000000100000000ull &&
                            v1.d[1] == 0x0000000100000000ull);
    vscbi(&v1, &v2, &v3, ES_64);
    check("vscbi(0-1): 64", !v1.d[0] && !v1.d[1]);
    vscbi(&v1, &v2, &v3, ES_128);
    check("vscbi(0-1): 128", !v1.d[0] && !v1.d[1]);
    vsbi(&v1, &v2, &v3, &v2, ES_128);
    check("vsbi(0-1)", v1.d[0] == 0xfffffffffffffffeull &&
                       v1.d[1] == 0xfffffffffffffffeull);
    vsbcbi(&v1, &v2, &v3, &v2, ES_128);
    check("vsbcbi(0-1)", !v1.d[0] && !v1.d[1]);

    /* 0-1 (borrow) */
    v3.d[0] = v3.d[1] = 1;
    vsbi(&v1, &v2, &v3, &v4, ES_128);
    check("vsbi(0-1) - borrow", v1.d[0] == 0xfffffffffffffffeull &&
                                v1.d[1] == 0xffffffffffffffffull);
    vsbcbi(&v1, &v2, &v3, &v4, ES_128);
    check("vsbcbi(0-1) - borrow", !v1.d[0] && !v1.d[1]);

    CHECK_SIGILL(vs(&v1, &v2, &v3, ES_128 + 1));
    CHECK_SIGILL(vscbi(&v1, &v2, &v3, ES_128 + 1));
    CHECK_SIGILL(vsbi(&v1, &v2, &v3, &v4, ES_8));
    CHECK_SIGILL(vsbi(&v1, &v2, &v3, &v4, ES_16));
    CHECK_SIGILL(vsbi(&v1, &v2, &v3, &v4, ES_32));
    CHECK_SIGILL(vsbi(&v1, &v2, &v3, &v4, ES_64));
    CHECK_SIGILL(vsbi(&v1, &v2, &v3, &v4, ES_128 + 1));
    CHECK_SIGILL(vsbcbi(&v1, &v2, &v3, &v4, ES_8));
    CHECK_SIGILL(vsbcbi(&v1, &v2, &v3, &v4, ES_16));
    CHECK_SIGILL(vsbcbi(&v1, &v2, &v3, &v4, ES_32));
    CHECK_SIGILL(vsbcbi(&v1, &v2, &v3, &v4, ES_64));
    CHECK_SIGILL(vsbcbi(&v1, &v2, &v3, &v4, ES_128 + 1));

    return 0;
}
