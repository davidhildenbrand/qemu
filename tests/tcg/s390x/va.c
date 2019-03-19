/*
 * QEMU TCG Tests - s390x VECTOR ADD *
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

static inline void vacc(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        uint8_t m4)
{
    asm volatile("vacc %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static void test_vacc(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0x0011223344556677ull,
        .d[1] = 0x8899aabbccddeeffull,
    };
    S390Vector v3 = {
        .d[0] = 0x0022446688aacceeull,
        .d[1] = 0xeeccaa8866442200ull,
    };

    vacc(&v1, &v2, &v3, ES_8);
    check("vacc: 8", v1.d[0] == 0x0000000000000101ull &&
                     v1.d[1] == 0x0101010101010100ull);
    vacc(&v1, &v2, &v3, ES_16);
    check("vacc: 16", v1.d[0] == 0x0000000000000001ull &&
                      v1.d[1] == 0x0001000100010001ull);
    vacc(&v1, &v2, &v3, ES_32);
    check("vacc: 32", v1.d[0] == 0x0000000000000000ull &&
                      v1.d[1] == 0x0000000100000001ull);
    vacc(&v1, &v2, &v3, ES_64);
    check("vacc: 64", v1.d[0] == 0x0000000000000000ull &&
                      v1.d[1] == 0x0000000000000001ull);
    vacc(&v1, &v2, &v3, ES_128);
    check("vacc: 128", v1.d[0] == 0x0000000000000000ull &&
                       v1.d[1] == 0x0000000000000000ull);
    v2.d[0] = -1ull;
    v2.d[1] = -1ull;
    v3.d[0] = 0;
    v3.d[1] = 1;
    vacc(&v1, &v2, &v3, ES_128);
    check("vacc: 128", v1.d[0] == 0x0000000000000000ull &&
                       v1.d[1] == 0x0000000000000001ull);
    CHECK_SIGILL(vacc(&v1, &v2, &v3, ES_128 + 1););
}

int main(void)
{
    test_vacc();
    return 0;
}
