/*
 * QEMU TCG Tests - s390x VECTOR MERGE (HIGH|LOW)
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

static inline void vmrh(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        uint8_t m4)
{
    asm volatile("vmrh %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline void vmrl(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        uint8_t m4)
{
    asm volatile("vmrl %[v1], %[v2], %[v3], %[m4]\n"
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

    vmrh(&v1, &v2, &v3, ES_8);
    check("vmrh: 8", v1.d[0] == 0x00ff11ee22dd33ccull &&
                     v1.d[1] == 0x44bb55aa66997788ull);
    vmrl(&v1, &v2, &v3, ES_8);
    check("vmrl: 8", v1.d[0] == 0x88779966aa55bb44ull &&
                     v1.d[1] == 0xcc33dd22ee11ff00ull);
    vmrh(&v1, &v2, &v3, ES_16);
    check("vmrh: 16", v1.d[0] == 0x0011ffee2233ddccull &&
                      v1.d[1] == 0x4455bbaa66779988ull);
    vmrl(&v1, &v2, &v3, ES_16);
    check("vmrl: 16", v1.d[0] == 0x88997766aabb5544ull &&
                      v1.d[1] == 0xccdd3322eeff1100ull);
    vmrh(&v1, &v2, &v3, ES_32);
    check("vmrh: 32", v1.d[0] == 0x00112233ffeeddccull &&
                      v1.d[1] == 0x44556677bbaa9988ull);
    vmrl(&v1, &v2, &v3, ES_32);
    check("vmrl: 32", v1.d[0] == 0x8899aabb77665544ull &&
                      v1.d[1] == 0xccddeeff33221100ull);
    vmrh(&v1, &v2, &v3, ES_64);
    check("vmrh: 64", v1.d[0] == 0x0011223344556677ull &&
                      v1.d[1] == 0xffeeddccbbaa9988ull);
    vmrl(&v1, &v2, &v3, ES_64);
    check("vmrl: 64", v1.d[0] == 0x8899aabbccddeeffull &&
                      v1.d[1] == 0x7766554433221100ull);

    CHECK_SIGILL(vmrh(&v1, &v2, &v3, ES_128));
    CHECK_SIGILL(vmrl(&v1, &v2, &v3, ES_128));
}
