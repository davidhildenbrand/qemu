/*
 * QEMU TCG Tests - s390x VECTOR ELEMENT ROTATE LEFT LOGICAL
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

static inline void verllv(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                          uint8_t m4)
{
    asm volatile("verllv %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline void verll(S390Vector *v1, S390Vector *v3, uint64_t b2,
                         uint8_t m4)
{
    asm volatile("verll %[v1], %[v3], 0(%[b2]), %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v3] "+v" (v3->v)
                 : [b2] "d" (b2),
                       [m4] "i" (m4));
}

static void test_verll(void)
{
    S390Vector v1 = {};
    S390Vector v3 = {
        .d[0] = 0xffeeddccbbaa9988ull,
        .d[1] = 0x7766554433221100ull,
    };

    /* Rotate by 0 - set all unused bits to 1 to test if they will be ignored */
    verll(&v1, &v3, 0 | -8ull, ES_8);
    check("verll(0): 8", v1.d[0] == 0xffeeddccbbaa9988ull &&
                         v1.d[1] == 0x7766554433221100ull);
    verll(&v1, &v3, 0 | -16ull, ES_16);
    check("verll(0): 16", v1.d[0] == 0xffeeddccbbaa9988ull &&
                          v1.d[1] == 0x7766554433221100ull);
    verll(&v1, &v3, 0 | -32ull, ES_32);
    check("verll(0): 32", v1.d[0] == 0xffeeddccbbaa9988ull &&
                          v1.d[1] == 0x7766554433221100ull);
    verll(&v1, &v3, 0 | -64ull, ES_64);
    check("verll(0): 64", v1.d[0] == 0xffeeddccbbaa9988ull &&
                          v1.d[1] == 0x7766554433221100ull);

    /* Rotate by half of element size in bits */
    verll(&v1, &v3, 4, ES_8);
    check("verll(4): 8", v1.d[0] == 0xffeeddccbbaa9988ull &&
                         v1.d[1] == 0x7766554433221100ull);
    verll(&v1, &v3, 8, ES_16);
    check("verll(8): 16", v1.d[0] == 0xeeffccddaabb8899ull &&
                          v1.d[1] == 0x6677445522330011ull);
    verll(&v1, &v3, 16, ES_32);
    check("verll(16): 32", v1.d[0] == 0xddccffee9988bbaaull &&
                           v1.d[1] == 0x5544776611003322ull);
    verll(&v1, &v3, 32, ES_64);
    check("verll(32): 64", v1.d[0] == 0xbbaa9988ffeeddccull &&
                          v1.d[1] == 0x3322110077665544ull);

    /* Rotate by maximum possible (element size in bits - 1) */
    verll(&v1, &v3, 7, ES_8);
    check("verll(7): 8", v1.d[0] == 0xff77ee66dd55cc44ull &&
                         v1.d[1] == 0xbb33aa2299118800ull);
    verll(&v1, &v3, 15, ES_16);
    check("verll(15): 16", v1.d[0] == 0x7ff76ee65dd54cc4ull &&
                           v1.d[1] == 0x3bb32aa219910880ull);
    verll(&v1, &v3, 31, ES_32);
    check("verll(31): 32", v1.d[0] == 0x7ff76ee65dd54cc4ull &&
                           v1.d[1] == 0x3bb32aa219910880ull);
    verll(&v1, &v3, 63, ES_64);
    check("verll(63): 64", v1.d[0] == 0x7ff76ee65dd54cc4ull &&
                           v1.d[1] == 0x3bb32aa219910880ull);

    CHECK_SIGILL(verll(&v1, &v3, 0, ES_128));
}

static void test_verllv(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0xffeeddccbbaa9988ull,
        .d[1] = 0x7766554433221100ull,
    };
    S390Vector v3 = {
        .d[0] = 0xffffffffffffffffull, /* rotate by maximum possible */
        .d[1] = 0x0000000000000000ull, /* rotate by 0 */
    };

    verllv(&v1, &v2, &v3, ES_8);
    check("verllv: 8", v1.d[0] == 0xff77ee66dd55cc44ull &&
                       v1.d[1] == 0x7766554433221100ull);
    verllv(&v1, &v2, &v3, ES_16);
    check("verllv: 16", v1.d[0] == 0x7ff76ee65dd54cc4ull &&
                        v1.d[1] == 0x7766554433221100ull);
    verllv(&v1, &v2, &v3, ES_32);
    check("verllv: 32", v1.d[0] == 0x7ff76ee65dd54cc4ull &&
                        v1.d[1] == 0x7766554433221100ull);
    verllv(&v1, &v2, &v3, ES_32);
    check("verllv: 64", v1.d[0] == 0x7ff76ee65dd54cc4ull &&
                        v1.d[1] == 0x7766554433221100ull);

    CHECK_SIGILL(verllv(&v1, &v2, &v3, ES_128));
}

int main(void)
{
    test_verll();
    test_verllv();

    return 0;
}
