/*
 * QEMU TCG Tests - s390x VECTOR SUM ACROSS *
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

static inline void vsum(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        uint8_t m4)
{
    asm volatile("vsum %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline void vsumg(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                         uint8_t m4)
{
    asm volatile("vsumg %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline void vsumq(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                         uint8_t m4)
{
    asm volatile("vsumq %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
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

    /* Add some random numbers */
    vsum(&v1, &v2, &v3, ES_8);
    check("vsum: 8", v1.d[0] == 0x00000132000001feull &&
                     v1.d[1] == 0x000002ca00000396ull);
    vsum(&v1, &v2, &v3, ES_16);
    check("vsum: 16", v1.d[0] == 0x0001001000014454ull &&
                      v1.d[1] == 0x000188980001ccdcull);
    vsumg(&v1, &v2, &v3, ES_16);
    check("vsumg: 16", v1.d[0] == 0x0000000000016698ull &&
                       v1.d[1] == 0x0000000000030030ull);
    vsumg(&v1, &v2, &v3, ES_32);
    check("vsumg: 32", v1.d[0] == 0x0000000100112232ull &&
                       v1.d[1] == 0x000000018899aabaull);
    vsumq(&v1, &v2, &v3, ES_32);
    check("vsumq: 32", v1.d[0] == 0x0000000000000000ull &&
                       v1.d[1] == 0x00000001cd003364ull);
    vsumq(&v1, &v2, &v3, ES_64);
    check("vsumq: 64", v1.d[0] == 0x0000000000000001ull &&
                       v1.d[1] == 0x0011223344556676ull);

    /* Add the maximum possible */
    v2.d[0] = v2.d[1] = v3.d[0] = v3.d[1] = -1ull;
    vsum(&v1, &v2, &v3, ES_8);
    check("vsum(max): 8", v1.d[0] == 0x000004fb000004fbull &&
                          v1.d[1] == 0x000004fb000004fbull);
    vsum(&v1, &v2, &v3, ES_16);
    check("vsum(max): 16", v1.d[0] == 0x0002fffd0002fffdull &&
                           v1.d[1] == 0x0002fffd0002fffdull);
    vsumg(&v1, &v2, &v3, ES_16);
    check("vsumg(max): 16", v1.d[0] == 0x000000000004fffbull &&
                            v1.d[1] == 0x000000000004fffbull);
    vsumg(&v1, &v2, &v3, ES_32);
    check("vsumg(max): 32", v1.d[0] == 0x00000002fffffffdull &&
                            v1.d[1] == 0x00000002fffffffdull);
    vsumq(&v1, &v2, &v3, ES_32);
    check("vsumq(max): 32", v1.d[0] == 0x0000000000000000ull &&
                            v1.d[1] == 0x00000004fffffffbull);
    vsumq(&v1, &v2, &v3, ES_64);
    check("vsumq(max): 64", v1.d[0] == 0x0000000000000002ull &&
                            v1.d[1] == 0xfffffffffffffffdull);

    CHECK_SIGILL(vsum(&v1, &v2, &v3, ES_32));
    CHECK_SIGILL(vsum(&v1, &v2, &v3, ES_64));
    CHECK_SIGILL(vsum(&v1, &v2, &v3, ES_128));
    CHECK_SIGILL(vsumg(&v1, &v2, &v3, ES_8));
    CHECK_SIGILL(vsumg(&v1, &v2, &v3, ES_64));
    CHECK_SIGILL(vsumg(&v1, &v2, &v3, ES_128));
    CHECK_SIGILL(vsumq(&v1, &v2, &v3, ES_8));
    CHECK_SIGILL(vsumq(&v1, &v2, &v3, ES_16));
    CHECK_SIGILL(vsumq(&v1, &v2, &v3, ES_128));
    return 0;
}
