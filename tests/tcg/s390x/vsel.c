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

static inline void vsel(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                        S390Vector *v4)
{
    asm volatile("vsel %[v1], %[v2], %[v3], %[v4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v),
                   [v4] "+v" (v4->v));
}

int main(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0xffffffffffffffffull,
        .d[1] = 0x0000000000000000ull,
    };
    S390Vector v3 = {
        .d[0] = 0x0000000000000000ull,
        .d[1] = 0xffffffffffffffffull,
    };
    S390Vector v4 = {
        .d[0] = 0x0123456789abcdefull,
        .d[1] = 0xfedcba9876543210ull,
    };

    vsel(&v1, &v2, &v3, &v4);
    check("vsel", v1.d[0] == 0x0123456789abcdefull &&
                  v1.d[1] == 0x0123456789abcdefull);
    return 0;
}
