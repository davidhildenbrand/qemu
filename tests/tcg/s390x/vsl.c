/*
 * QEMU TCG Tests - s390x VECTOR SHIFT *
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

static inline void vsl(S390Vector *v1, S390Vector *v2, S390Vector *v3)
{
    asm volatile("vsl %[v1], %[v2], %[v3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v));
}

static inline void vslb(S390Vector *v1, S390Vector *v2, S390Vector *v3)
{
    asm volatile("vslb %[v1], %[v2], %[v3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v));
}

static inline void vsldb(S390Vector *v1, S390Vector *v2, S390Vector *v3,
                         uint8_t i4)
{
    asm volatile("vsldb %[v1], %[v2], %[v3], %[i4]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v)
                 : [i4] "i" (i4));
}

static inline void vsra(S390Vector *v1, S390Vector *v2, S390Vector *v3)
{
    asm volatile("vsra %[v1], %[v2], %[v3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v));
}

static inline void vsrab(S390Vector *v1, S390Vector *v2, S390Vector *v3)
{
    asm volatile("vsrab %[v1], %[v2], %[v3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v));
}

static inline void vsrl(S390Vector *v1, S390Vector *v2, S390Vector *v3)
{
    asm volatile("vsrl %[v1], %[v2], %[v3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v));
}

static inline void vsrlb(S390Vector *v1, S390Vector *v2, S390Vector *v3)
{
    asm volatile("vsrlb %[v1], %[v2], %[v3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v),
                   [v3] "+v" (v3->v));
}

int main(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0xffeeddccbbaa9988ull,
        .d[1] = 0x7766554433221100ull,
    };
    S390Vector v3 = { };

    /* Shift by 0 */
    vsl(&v1, &v2, &v3);
    check("vsl(0)", v1.d[0] == 0xffeeddccbbaa9988ull &&
                    v1.d[1] == 0x7766554433221100ull);
    vslb(&v1, &v2, &v3);
    check("vslb(0)", v1.d[0] == 0xffeeddccbbaa9988ull &&
                     v1.d[1] == 0x7766554433221100ull);
    vsra(&v1, &v2, &v3);
    check("vsra(0)", v1.d[0] == 0xffeeddccbbaa9988ull &&
                     v1.d[1] == 0x7766554433221100ull);
    vsrab(&v1, &v2, &v3);
    check("vsrab(0)", v1.d[0] == 0xffeeddccbbaa9988ull &&
                      v1.d[1] == 0x7766554433221100ull);
    vsrl(&v1, &v2, &v3);
    check("vsrl(0)", v1.d[0] == 0xffeeddccbbaa9988ull &&
                     v1.d[1] == 0x7766554433221100ull);
    vsrlb(&v1, &v2, &v3);
    check("vsrlb(0)", v1.d[0] == 0xffeeddccbbaa9988ull &&
                      v1.d[1] == 0x7766554433221100ull);

    /* Shift by 1 bit */
    v3.d[0] = v3.d[1] = 0x0101010101010101ull;
    vsl(&v1, &v2, &v3);
    check("vsl(1)", v1.d[0] == 0xffddbb9977553310ull &&
                    v1.d[1] == 0xeeccaa8866442200ull);
    vsra(&v1, &v2, &v3);
    check("vsra(1)", v1.d[0] == 0xfff76ee65dd54cc4ull &&
                     v1.d[1] == 0x3bb32aa219910880ull);
    vsrl(&v1, &v2, &v3);
    check("vsrl(1)", v1.d[0] == 0x7ff76ee65dd54cc4ull &&
                     v1.d[1] == 0x3bb32aa219910880ull);

    /* Shift by one byte */
    v3.d[0] = 8;
    v3.d[1] = 0;
    vslb(&v1, &v2, &v3);
    check("vslb(1)", v1.d[0] == 0xeeddccbbaa998877ull &&
                     v1.d[1] == 0x6655443322110000ull);
    vsrab(&v1, &v2, &v3);
    check("vsrab(1)", v1.d[0] == 0xffffeeddccbbaa99ull &&
                      v1.d[1] == 0x8877665544332211ull);
    vsrlb(&v1, &v2, &v3);
    check("vsrlb(1)", v1.d[0] == 0x00ffeeddccbbaa99ull &&
                      v1.d[1] == 0x8877665544332211ull);

    /* Shift by 8 bytes */
    v3.d[0] = 64;
    v3.d[1] = 0;
    vslb(&v1, &v2, &v3);
    check("vslb(8)", v1.d[0] == 0x7766554433221100ull && !v1.d[1]);
    vsrab(&v1, &v2, &v3);
    check("vsrab(8)", v1.d[0] == 0xffffffffffffffffull &&
                      v1.d[1] == 0xffeeddccbbaa9988ull);
    vsrlb(&v1, &v2, &v3);
    check("vsrlb(8)", !v1.d[0] && v1.d[1] == 0xffeeddccbbaa9988ull);

    /* Shift by 9 bytes */
    v3.d[0] = 72;
    v3.d[1] = 0;
    vslb(&v1, &v2, &v3);
    check("vslb(9)", v1.d[0] == 0x6655443322110000ull && !v1.d[1]);
    vsrab(&v1, &v2, &v3);
    check("vsrab(9)", v1.d[0] == 0xffffffffffffffffull &&
                      v1.d[1] == 0xffffeeddccbbaa99ull);
    vsrlb(&v1, &v2, &v3);
    check("vsrlb(9)", !v1.d[0] && v1.d[1] == 0x00ffeeddccbbaa99ull);

    /* Shift by max (test if bits get ignored) */
    v3.d[0] = v3.d[1] = -1ull;
    vsl(&v1, &v2, &v3);
    check("vsl(max)", v1.d[0] == 0xf76ee65dd54cc43bull &&
                      v1.d[1] == 0xb32aa21991088000ull);
    vslb(&v1, &v2, &v3);
    check("vslb(max)", !v1.d[0] && !v1.d[1]);
    vsra(&v1, &v2, &v3);
    check("vsra(max)", v1.d[0] == 0xffffddbb99775533ull &&
                       v1.d[1] == 0x10eeccaa88664422ull);
    vsrab(&v1, &v2, &v3);
    check("vsrab(max)", v1.d[0] == -1ull && v1.d[1] == -1ull);
    vsrl(&v1, &v2, &v3);
    check("vsrl(max)", v1.d[0] == 0x01ffddbb99775533ull &&
                       v1.d[1] == 0x10eeccaa88664422ull);
    vsrlb(&v1, &v2, &v3);
    check("vsrlb(max)", !v1.d[0] && v1.d[1] == 0x00000000000000ffull);

    /* VSLDB is different to the other functions */
    v2.d[0] = 0xffffeeeeddddccccull;
    v2.d[1] = 0xbbbbaaaa99998888ull;
    v3.d[0] = 0x7777666655554444ull;
    v3.d[1] = 0x3333222211110000ull;
    vsldb(&v1, &v2, &v3, 0);
    check("vsldb(0)", v1.d[0] == 0xffffeeeeddddccccull &&
                      v1.d[1] == 0xbbbbaaaa99998888ull);
    vsldb(&v1, &v2, &v3, 1);
    check("vsldb(1)", v1.d[0] == 0xffeeeeddddccccbbull &&
                      v1.d[1] == 0xbbaaaa9999888877ull);
    vsldb(&v1, &v2, &v3, 7);
    check("vsldb(7)", v1.d[0] == 0xccbbbbaaaa999988ull &&
                      v1.d[1] == 0x8877776666555544ull);
    vsldb(&v1, &v2, &v3, 8);
    check("vsldb(8)", v1.d[0] == 0xbbbbaaaa99998888ull &&
                      v1.d[1] == 0x7777666655554444ull);
    vsldb(&v1, &v2, &v3, 15);
    check("vsldb(15)", v1.d[0] == 0x8877776666555544ull &&
                       v1.d[1] == 0x4433332222111100ull);

    return 0;
}
