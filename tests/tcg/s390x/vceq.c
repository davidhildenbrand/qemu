/*
 * QEMU TCG Tests - s390x VECTOR COMPARE EQUAL
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

static inline int vceq(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                       uint8_t i4)
{
    int cc;

    /* Always modify the CC */
    asm volatile("vceq %[v1], %[v2], %[v3], %[i4], 1\n"
                 "ipm %[cc]\n"
                 "srl %[cc], 28\n"
                 : [cc] "=d" (cc),
                   [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [i4] "i" (i4)
                 : "cc");

    return cc;
}

int main(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0xffeeddccbbaa9988ull,
        .d[1] = 0x7766554433221100ull,
    };
    S390Vector v3 = {
        .d[0] = 0x7766554433221100ull,
        .d[1] = 0xffeeddccbbaa9988ull,
    };
    int cc;

    /* All elements are equal */
    cc = vceq(&v1, &v2, &v2, ES_8);
    check("vceq(all equal): 8", cc == 0 && v1.d[0] == -1ull &&
                                v1.d[1] == -1ull);
    cc = vceq(&v1, &v2, &v2, ES_16);
    check("vceq(all equal): 16", cc == 0 && v1.d[0] == -1ull &&
                                 v1.d[1] == -1ull);
    cc = vceq(&v1, &v2, &v2, ES_32);
    check("vceq(all equal): 32", cc == 0 && v1.d[0] == -1ull &&
                                 v1.d[1] == -1ull);
    cc = vceq(&v1, &v2, &v2, ES_64);
    check("vceq(all equal): 64", cc == 0 && v1.d[0] == -1ull &&
                                 v1.d[1] == -1ull);
    /* No elements are equal */
    cc = vceq(&v1, &v2, &v3, ES_8);
    check("vceq(non equal): 8", cc == 3 && !v1.d[0] && !v1.d[1]);
    cc = vceq(&v1, &v2, &v3, ES_16);
    check("vceq(non equal): 16", cc == 3 && !v1.d[0] && !v1.d[1]);
    cc = vceq(&v1, &v2, &v3, ES_32);
    check("vceq(non equal): 32", cc == 3 && !v1.d[0] && !v1.d[1]);
    cc = vceq(&v1, &v2, &v3, ES_64);
    check("vceq(non equal): 64", cc == 3 && !v1.d[0] && !v1.d[1]);

    /* Some elements are equal */
    v3.d[1] = v2.d[1];
    cc = vceq(&v1, &v2, &v3, ES_8);
    check("vceq(some equal): 8", cc == 1 && !v1.d[0] && v1.d[1] == -1ull);
    cc = vceq(&v1, &v2, &v3, ES_16);
    check("vceq(some equal): 16", cc == 1 && !v1.d[0] && v1.d[1] == -1ull);
    cc = vceq(&v1, &v2, &v3, ES_32);
    check("vceq(some equal): 32", cc == 1 && !v1.d[0] && v1.d[1] == -1ull);
    cc = vceq(&v1, &v2, &v3, ES_64);
    check("vceq(some equal): 64", cc == 1 && !v1.d[0] && v1.d[1] == -1ull);

    CHECK_SIGILL(vceq(&v1, &v2, &v2, ES_128));
    return 0;
}
