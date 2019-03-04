/*
 * QEMU TCG Tests - s390x VECTOR GENERATE MASK
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

static inline void vgm(S390Vector *v1, uint8_t i2, uint8_t i3, uint8_t m4)
{
    asm volatile("vgm %[v1], %[i2], %[i3], %[m4]\n"
                 : [v1] "+v" (v1->v)
                 : [i2] "i" (i2),
                   [i3] "i" (i3),
                   [m4] "i" (m4));
}

int main(void)
{
    S390Vector v1 = {};

    vgm(&v1, 6, 6, ES_8);
    check("8: single", v1.d[0] == 0x0202020202020202ull &&
                       v1.d[1] == 0x0202020202020202ull);
    vgm(&v1, 1, 6, ES_8);
    check("8: range", v1.d[0] == 0x7e7e7e7e7e7e7e7eull &&
                      v1.d[1] == 0x7e7e7e7e7e7e7e7eull);
    vgm(&v1, 7, 0, ES_8);
    check("8: wrapping", v1.d[0] == 0x8181818181818181ull &&
                         v1.d[1] == 0x8181818181818181ull);
    vgm(&v1, 60, 63, ES_8);
    check("8: unused", v1.d[0] == 0x0f0f0f0f0f0f0f0full &&
                       v1.d[1] == 0x0f0f0f0f0f0f0f0full);
    vgm(&v1, 14, 14, ES_16);
    check("16: single", v1.d[0] == 0x0002000200020002ull &&
                        v1.d[1] == 0x0002000200020002ull);
    vgm(&v1, 1, 14, ES_16);
    check("16: range", v1.d[0] == 0x7ffe7ffe7ffe7ffeull &&
                       v1.d[1] == 0x7ffe7ffe7ffe7ffeull);
    vgm(&v1, 15, 0, ES_16);
    check("16: wrapping", v1.d[0] == 0x8001800180018001ull &&
                          v1.d[1] == 0x8001800180018001ull);
    vgm(&v1, 60, 63, ES_16);
    check("16: unused", v1.d[0] == 0x000f000f000f000full &&
                        v1.d[1] == 0x000f000f000f000full);
    vgm(&v1, 30, 30, ES_32);
    check("32: single", v1.d[0] == 0x0000000200000002ull &&
                       v1.d[1] == 0x0000000200000002ull);
    vgm(&v1, 1, 30, ES_32);
    check("32: range", v1.d[0] == 0x7ffffffe7ffffffeull &&
                       v1.d[1] == 0x7ffffffe7ffffffeull);
    vgm(&v1, 31, 0, ES_32);
    check("32: wrapping", v1.d[0] == 0x8000000180000001ull &&
                          v1.d[1] == 0x8000000180000001ull);
    vgm(&v1, 60, 63, ES_32);
    check("32: unused", v1.d[0] == 0x0000000f0000000full &&
                        v1.d[1] == 0x0000000f0000000full);
    vgm(&v1, 62, 62, ES_64);
    check("64: single", v1.d[0] == 0x2 && v1.d[1] == 0x2);
    vgm(&v1, 1, 62, ES_64);
    check("64: range", v1.d[0] == 0x7ffffffffffffffeull &&
                       v1.d[1] == 0x7ffffffffffffffeull);
    vgm(&v1, 63, 0, ES_64);
    check("64: wrapping", v1.d[0] == 0x8000000000000001ull &&
                          v1.d[1] == 0x8000000000000001ull);
    CHECK_SIGILL(vgm(&v1, 0, 0, ES_128));
    return 0;
}
