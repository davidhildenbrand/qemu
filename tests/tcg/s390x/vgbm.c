/*
 * QEMU TCG Tests - s390x VECTOR GENERATE BYTE MASK
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
#include "helper.h"

static inline void vgbm(S390Vector *v1, uint16_t i2)
{
    asm volatile("vgbm %[v1], %[i2]\n"
                 : [v1] "+v" (v1->v)
                 : [i2] "i" (i2));
}

int main(void)
{
    S390Vector v1 = {};

    vgbm(&v1, 0x0000);
    check("i2 == 0x0000", v1.d[0] == 0 && v1.d[1] == 0);
    vgbm(&v1, 0x00ff);
    check("i2 == 0x00ff", v1.d[0] == 0 && v1.d[1] == -1ull);
    vgbm(&v1, 0x0f00);
    check("i2 == 0x0f00", v1.d[0] == 0x00000000ffffffffull &&
                          v1.d[1] == 0);
    vgbm(&v1, 0x5555);
    check("i2 == 0x5555", v1.d[0] == 0x00ff00ff00ff00ffull &&
                          v1.d[1] == 0x00ff00ff00ff00ffull);
    vgbm(&v1, 0x4218);
    check("i2 == 0x4218", v1.d[0] == 0x00ff00000000ff00ull &&
                          v1.d[1] == 0x000000ffff000000ull);
    return 0;
}
