/*
 * QEMU TCG Tests - s390x VECTOR PACK
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

static inline void vpk(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                       uint8_t m4)
{
    asm volatile("vpk %[v1], %[v2], %[v3], %[m4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [m4] "i" (m4));
}

static inline int vpks(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                       uint8_t m4, uint8_t m5)
{
    int cc = 0;

    asm volatile("vpks %[v1], %[v2], %[v3], %[m4], %[m5]\n"
                 "ipm %[cc]\n"
                 "srl %[cc],28\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v),
                   [cc] "=d" (cc)
                 : [m4] "i" (m4),
                   [m5] "i" (m5));
    return cc;
}

static inline int vpkls(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        uint8_t m4, uint8_t m5)
{
    int cc = 0;

    asm volatile("vpkls %[v1], %[v2], %[v3], %[m4], %[m5]\n"
                 "ipm %[cc]\n"
                 "srl %[cc],28\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v),
                   [cc] "=d" (cc)
                 : [m4] "i" (m4),
                   [m5] "i" (m5));
    return cc;
}

static void test_vpk(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0x0000111122223333ull,
        .d[1] = 0x4444555566667777ull,
    };
    S390Vector v3 = {
        .d[0] = 0x88889999aaaabbbbull,
        .d[1] = 0xccccddddeeeeffffull,
    };

    vpk(&v1, &v2, &v3, ES_16);
    check("vpk: 16->8", v1.d[0] == 0x0011223344556677ull &&
                        v1.d[1] == 0x8899aabbccddeeffull);
    vpk(&v1, &v2, &v3, ES_32);
    check("vpk: 32->16", v1.d[0] == 0x1111333355557777ull &&
                         v1.d[1] == 0x9999bbbbddddffffull);
    vpk(&v1, &v2, &v3, ES_64);
    check("vpk: 64->32", v1.d[0] == 0x2222333366667777ull &&
                         v1.d[1] == 0xaaaabbbbeeeeffffull);

    CHECK_SIGILL(vpk(&v1, &v2, &v3, ES_8));
    CHECK_SIGILL(vpk(&v1, &v2, &v3, ES_128));
}

static void test_vpks(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {};
    S390Vector v3 = {};
    int cc, i;

    vpks(&v1, &v2, &v3, ES_16, 0);
    check("vpks: 16->8, no sat", v1.d[0] == 0 && v1.d[1] == 0);
    cc = vpks(&v1, &v2, &v3, ES_16, 1);
    check("vpks: 16->8, no sat, cc", v1.d[0] == 0 && v1.d[1] == 0 && cc == 0);
    vpks(&v1, &v2, &v3, ES_32, 0);
    check("vpks: 32->16, no sat", v1.d[0] == 0 && v1.d[1] == 0);
    cc = vpks(&v1, &v2, &v3, ES_32, 1);
    check("vpks: 32->16, no sat, cc", v1.d[0] == 0 && v1.d[1] == 0 && cc == 0);
    vpks(&v1, &v2, &v3, ES_64, 0);
    check("vpks: 64->32, no sat", v1.d[0] == 0 && v1.d[1] == 0);
    cc = vpks(&v1, &v2, &v3, ES_64, 1);
    check("vpks: 64->32, no sat, cc", v1.d[0] == 0 && v1.d[1] == 0 && cc == 0);
    v2.d[1] = (uint16_t)INT16_MIN;
    v3.d[1] = (uint16_t)INT16_MAX;
    vpks(&v1, &v2, &v3, ES_16, 0);
    check("vpks: 16->8, some sat", v1.d[0] == (uint8_t)INT8_MIN &&
                                   v1.d[1] == (uint8_t)INT8_MAX);
    cc = vpks(&v1, &v2, &v3, ES_16, 1);
    check("vpks: 16->8, some sat, cc", v1.d[0] == (uint8_t)INT8_MIN &&
                                       v1.d[1] == (uint8_t)INT8_MAX && cc == 1);
    v2.d[1] = (uint32_t)INT32_MIN;
    v3.d[1] = (uint32_t)INT32_MAX;
    vpks(&v1, &v2, &v3, ES_32, 0);
    check("vpks: 32->16, some sat", v1.d[0] == (uint16_t)INT16_MIN &&
                                    v1.d[1] == (uint16_t)INT16_MAX);
    cc = vpks(&v1, &v2, &v3, ES_32, 1);
    check("vpks: 32->16, some sat, cc", v1.d[0] == (uint16_t)INT16_MIN &&
                                        v1.d[1] == (uint16_t)INT16_MAX &&
                                        cc == 1);
    v2.d[1] = (uint64_t)INT64_MIN;
    v3.d[1] = (uint64_t)INT64_MAX;
    vpks(&v1, &v2, &v3, ES_64, 0);
    check("vpks: 64->32, some sat", v1.d[0] == (uint32_t)INT32_MIN &&
                                    v1.d[1] == (uint32_t)INT32_MAX);
    cc = vpks(&v1, &v2, &v3, ES_64, 1);
    check("vpks: 64->32, some sat, cc", v1.d[0] == (uint32_t)INT32_MIN &&
                                        v1.d[1] == (uint32_t)INT32_MAX &&
                                        cc == 1);
    for (i = 0; i < 8; i++) {
        v2.h[i] = (uint16_t)INT16_MAX;
        v3.h[i] = (uint16_t)INT16_MIN;
    }
    vpks(&v1, &v2, &v3, ES_16, 0);
    check("vpks: 16->8, all sat", v1.b[0] == (uint8_t)INT8_MAX &&
                                  v1.b[8] == (uint8_t)INT8_MIN);
    cc = vpks(&v1, &v2, &v3, ES_16, 1);
    check("vpks: 16->8, all sat, cc", v1.b[0] == (uint8_t)INT8_MAX &&
                                      v1.b[8] == (uint8_t)INT8_MIN && cc == 3);
    for (i = 0; i < 4; i++) {
        v2.w[i] = (uint32_t)INT32_MAX;
        v3.w[i] = (uint32_t)INT32_MIN;
    }
    vpks(&v1, &v2, &v3, ES_32, 0);
    check("vpks: 32->16, all sat", v1.h[0] == (uint16_t)INT16_MAX &&
                                   v1.h[4] == (uint16_t)INT16_MIN);
    cc = vpks(&v1, &v2, &v3, ES_32, 1);
    check("vpks: 32->16, all sat, cc", v1.h[0] == (uint16_t)INT16_MAX &&
                                       v1.h[4] == (uint16_t)INT16_MIN &&
                                       cc == 3);
    for (i = 0; i < 2; i++) {
        v2.d[i] = (uint64_t)INT64_MAX;
        v3.d[i] = (uint64_t)INT64_MIN;
    }
    vpks(&v1, &v2, &v3, ES_64, 0);
    check("vpks: 64->32, all sat", v1.w[0] == (uint32_t)INT32_MAX &&
                                   v1.w[2] == (uint32_t)INT32_MIN);
    cc = vpks(&v1, &v2, &v3, ES_64, 1);
    check("vpks: 64->32, all sat, cc", v1.w[0] == (uint32_t)INT32_MAX &&
                                       v1.w[2] == (uint32_t)INT32_MIN &&
                                       cc == 3);

    CHECK_SIGILL(vpks(&v1, &v2, &v3, ES_8, 0));
    CHECK_SIGILL(vpks(&v1, &v2, &v3, ES_128, 0));
}

static void test_vpkls(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {};
    S390Vector v3 = {};
    int cc, i;

    vpkls(&v1, &v2, &v3, ES_16, 0);
    check("vpkls: 16->8, no sat", v1.d[0] == 0 && v1.d[1] == 0);
    cc = vpkls(&v1, &v2, &v3, ES_16, 1);
    check("vpkls: 16->8, no sat, cc", v1.d[0] == 0 && v1.d[1] == 0 && cc == 0);
    vpkls(&v1, &v2, &v3, ES_32, 0);
    check("vpkls: 32->16, no sat", v1.d[0] == 0 && v1.d[1] == 0);
    cc = vpkls(&v1, &v2, &v3, ES_32, 1);
    check("vpkls: 32->16, no sat, cc", v1.d[0] == 0 && v1.d[1] == 0 && cc == 0);
    vpkls(&v1, &v2, &v3, ES_64, 0);
    check("vpkls: 64->32, no sat", v1.d[0] == 0 && v1.d[1] == 0);
    cc = vpkls(&v1, &v2, &v3, ES_64, 1);
    check("vpkls: 64->32, no sat, cc", v1.d[0] == 0 && v1.d[1] == 0 && cc == 0);
    v2.d[1] = 123;
    v3.d[1] = UINT16_MAX - 20;
    vpkls(&v1, &v2, &v3, ES_16, 0);
    check("vpkls: 16->8, some sat", v1.d[0] == 123 && v1.d[1] == UINT8_MAX);
    cc = vpkls(&v1, &v2, &v3, ES_16, 1);
    check("vpkls: 16->8, some sat, cc", v1.d[0] == 123 &&
                                        v1.d[1] == UINT8_MAX && cc == 1);
    v2.d[1] = 1234;
    v3.d[1] = UINT32_MAX - 200;
    vpkls(&v1, &v2, &v3, ES_32, 0);
    check("vpkls: 32->16, some sat", v1.d[0] == 1234 && v1.d[1] == UINT16_MAX);
    cc = vpkls(&v1, &v2, &v3, ES_32, 1);
    check("vpkls: 32->16, some sat, cc", v1.d[0] == 1234 &&
                                         v1.d[1] == UINT16_MAX && cc == 1);
    v2.d[1] = 12345;
    v3.d[1] = UINT64_MAX - 2000;
    vpkls(&v1, &v2, &v3, ES_64, 0);
    check("vpkls: 64->32, some sat", v1.d[0] == 12345 && v1.d[1] == UINT32_MAX);
    cc = vpkls(&v1, &v2, &v3, ES_64, 1);
    check("vpkls: 64->32, some sat, cc", v1.d[0] == 12345 &&
                                         v1.d[1] == UINT32_MAX && cc == 1);
    for (i = 0; i < 8; i++) {
        v2.h[i] = UINT16_MAX - 20;
        v3.h[i] = UINT16_MAX - 400;
    }
    vpkls(&v1, &v2, &v3, ES_16, 0);
    check("vpkls: 16->8, all sat", v1.d[0] == -1ull && v1.d[1] == -1ull);
    cc = vpkls(&v1, &v2, &v3, ES_16, 1);
    check("vpkls: 16->8, all sat, cc", v1.d[0] == -1ull && v1.d[1] == -1ull &&
                                       cc == 3);
    for (i = 0; i < 4; i++) {
        v2.w[i] = UINT32_MAX - 20;
        v3.w[i] = UINT32_MAX - 40000;
    }
    vpkls(&v1, &v2, &v3, ES_32, 0);
    check("vpkls: 32->16, all sat", v1.d[0] == -1ull && v1.d[1] == -1ull);
    cc = vpkls(&v1, &v2, &v3, ES_32, 1);
    check("vpkls: 32->16, all sat, cc", v1.d[0] == -1ull && v1.d[1] == -1ull &&
                                        cc == 3);
    for (i = 0; i < 2; i++) {
        v2.d[i] = UINT64_MAX - 20;
        v3.d[i] = UINT64_MAX - 400000;
    }
    vpkls(&v1, &v2, &v3, ES_64, 0);
    check("vpkls: 64->32, all sat", v1.d[0] == -1ull && v1.d[1] == -1ull);
    cc = vpkls(&v1, &v2, &v3, ES_64, 1);
    check("vpkls: 64->32, all sat, cc", v1.d[0] == -1ull && v1.d[1] == -1ull &&
                                        cc == 3);

    CHECK_SIGILL(vpkls(&v1, &v2, &v3, ES_8, 0));
    CHECK_SIGILL(vpkls(&v1, &v2, &v3, ES_128, 0));
}

int main(void)
{
    test_vpk();
    test_vpks();
    test_vpkls();
}
