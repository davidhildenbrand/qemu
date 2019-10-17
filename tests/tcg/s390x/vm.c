/*
 * QEMU TCG Tests - s390x VECTOR MULTIPLY *
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

static inline void vmlo(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        uint8_t m4)
{
    asm volatile("vmlo %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline void vmalo(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                         S390Vector *v4, uint8_t m4)
{
    asm volatile("vmalo %[v1], %[v2], %[v3], %[v4], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v),
                   [v4] "+v" (v4->v)
                 : [m4] "i" (m4));
}

static void test_vmlo(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0xffeeddccbbaa9988ull,
        .d[1] = 0x7766554433221100ull,
    };
    S390Vector v3 = {
        .d[0] = 0x0011223344556677ull,
        .d[1] = 0x8899aabbccddeeffull,
    };
    S390Vector v4 = {
        .d[0] = 0x7766554433221100ull,
        .d[1] = 0xffeeddccbbaa9988ull,
    };

    /* Random numbers */
    vmlo(&v1, &v2, &v3, ES_8);
    check("vmlo: 8", v1.d[0] == 0x0fce28a438723f38ull &&
                     v1.d[1] == 0x3cf631ac1d5a0000ull);
    vmlo(&v1, &v2, &v3, ES_16);
    check("vmlo: 16", v1.d[0] == 0x1da147a43d738e38ull &&
                      v1.d[1] == 0x38dd70ac0fdeef00ull);
    vmlo(&v1, &v2, &v3, ES_32);
    check("vmlo: 32", v1.d[0] == 0x3217eb8a4fa18e38ull &&
                      v1.d[1] == 0x28eb79b147bcef00ull);

    vmalo(&v1, &v2, &v3, &v4, ES_8);
    check("vmalo: 8", v1.d[0] == 0x87347de86b945038ull &&
                      v1.d[1] == 0x3ce40f78d9049988ull);
    vmalo(&v1, &v2, &v3, &v4, ES_16);
    check("vmalo: 16", v1.d[0] == 0x95079ce870959f38ull &&
                       v1.d[1] == 0x38cc4e78cb898888ull);
    vmalo(&v1, &v2, &v3, &v4, ES_32);
    check("vmalo: 32", v1.d[0] == 0xa97e40ce82c39f38ull &&
                       v1.d[1] == 0x28da577e03678888ull);

    /* Multiply with zero, add zero */
    v3.d[0] = v3.d[1] = v4.d[0] = v4.d[1] = 0;
    vmlo(&v1, &v2, &v3, ES_8);
    check("vmlo(0): 8", !v1.d[0] && !v1.d[1]);
    vmlo(&v1, &v2, &v3, ES_16);
    check("vmlo(0): 16", !v1.d[0] && !v1.d[1]);
    vmlo(&v1, &v2, &v3, ES_32);
    check("vmlo(0): 32", !v1.d[0] && !v1.d[1]);

    vmalo(&v1, &v2, &v3, &v4, ES_8);
    check("vmalo(0): 8", !v1.d[0] && !v1.d[1]);
    vmalo(&v1, &v2, &v3, &v4, ES_16);
    check("vmalo(0): 16", !v1.d[0] && !v1.d[1]);
    vmalo(&v1, &v2, &v3, &v4, ES_32);
    check("vmalo(0): 32", !v1.d[0] && !v1.d[1]);

    /* Multiply with maximum possible and add maxium possible*/
    v3.d[0] = v3.d[1] = v4.d[0] = v4.d[1] = -1ull;
    vmlo(&v1, &v2, &v3, ES_8);
    check("vmlo(max): 8", v1.d[0] == 0xed12cb34a9568778ull &&
                          v1.d[1] == 0x659a43bc21de0000ull);
    vmlo(&v1, &v2, &v3, ES_16);
    check("vmlo(max): 16", v1.d[0] == 0xddcb223499876678ull &&
                           v1.d[1] == 0x5543aabc10ffef00ull);
    vmlo(&v1, &v2, &v3, ES_32);
    check("vmlo(max): 32", v1.d[0] == 0xbbaa998744556678ull &&
                           v1.d[1] == 0x332210ffccddef00ull);

    vmalo(&v1, &v2, &v3, &v4, ES_8);
    check("vmalo(max): 8", v1.d[0] == 0xed11cb33a9558777ull &&
                           v1.d[1] == 0x659943bb21ddffffull);
    vmalo(&v1, &v2, &v3, &v4, ES_16);
    check("vmalo(max): 16", v1.d[0] == 0xddcb223399876677ull &&
                            v1.d[1] == 0x5543aabb10ffeeffull);
    vmalo(&v1, &v2, &v3, &v4, ES_32);
    check("vmalo(max): 32", v1.d[0] == 0xbbaa998744556677ull &&
                            v1.d[1] == 0x332210ffccddeeffull);

    CHECK_SIGILL(vmlo(&v1, &v2, &v3, ES_64););
    CHECK_SIGILL(vmlo(&v1, &v2, &v3, ES_128););
    CHECK_SIGILL(vmalo(&v1, &v2, &v3, &v4, ES_64););
    CHECK_SIGILL(vmalo(&v1, &v2, &v3, &v4, ES_128););
}

int main(void)
{
    test_vmlo();
    return 0;
}
