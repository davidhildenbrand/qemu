/*
 * QEMU TCG Tests - s390x VECTOR UNPACK
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

static inline void vuph(S390Vector *v1, S390Vector *v2, uint8_t m3)
{
    asm volatile("vuph %[v1], %[v2], %[m3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v)
                 : [m3] "i" (m3));
}

static inline void vuplh(S390Vector *v1, S390Vector *v2, uint8_t m3)
{
    asm volatile("vuplh %[v1], %[v2], %[m3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v)
                 : [m3] "i" (m3));
}

static inline void vupl(S390Vector *v1, S390Vector *v2, uint8_t m3)
{
    asm volatile("vupl %[v1], %[v2], %[m3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v)
                 : [m3] "i" (m3));
}

static inline void vupll(S390Vector *v1, S390Vector *v2, uint8_t m3)
{
    asm volatile("vupll %[v1], %[v2], %[m3]\n"
                 : [v1] "+v" (v1->v),
                   [v2] "+v" (v2->v)
                 : [m3] "i" (m3));
}

int main(void)
{
    S390Vector v1 = {};
    S390Vector v2 = {
        .d[0] = 0xf12345671234567full,
        .d[1] = 0xf76543217654321full,
    };

    vuph(&v1, &v2, ES_8);
    check("vuph: 8->16", v1.d[0] == 0xfff1002300450067ull &&
                         v1.d[1] == 0x001200340056007full);
    vuplh(&v1, &v2, ES_8);
    check("vuplh: 8->16", v1.d[0] == 0x00f1002300450067ull &&
                          v1.d[1] == 0x001200340056007full);
    vupl(&v1, &v2, ES_8);
    check("vupl: 8->16", v1.d[0] == 0xfff7006500430021ull &&
                         v1.d[1] == 0x007600540032001full);
    vupll(&v1, &v2, ES_8);
    check("vupll: 8->16", v1.d[0] == 0x00f7006500430021ull &&
                          v1.d[1] == 0x007600540032001full);

    vuph(&v1, &v2, ES_16);
    check("vuph: 16->32", v1.d[0] == 0xfffff12300004567ull &&
                          v1.d[1] == 0x000012340000567full);
    vuplh(&v1, &v2, ES_16);
    check("vuplh: 16->32", v1.d[0] == 0x0000f12300004567ull &&
                           v1.d[1] == 0x000012340000567full);
    vupl(&v1, &v2, ES_16);
    check("vupl: 16->32", v1.d[0] == 0xfffff76500004321ull &&
                          v1.d[1] == 0x000076540000321full);
    vupll(&v1, &v2, ES_16);
    check("vupll: 16->32", v1.d[0] == 0x0000f76500004321ull &&
                           v1.d[1] == 0x000076540000321full);

    vuph(&v1, &v2, ES_32);
    check("vuph: 32->64", v1.d[0] == 0xfffffffff1234567ull &&
                          v1.d[1] == 0x000000001234567full);
    vuplh(&v1, &v2, ES_32);
    check("vuplh: 32->64", v1.d[0] == 0x00000000f1234567ull &&
                           v1.d[1] == 0x000000001234567full);
    vupl(&v1, &v2, ES_32);
    check("vupl: 32->64", v1.d[0] == 0xfffffffff7654321ull &&
                          v1.d[1] == 0x000000007654321full);
    vupll(&v1, &v2, ES_32);
    check("vupll: 32->64", v1.d[0] == 0x00000000f7654321ull &&
                           v1.d[1] == 0x000000007654321full);

    CHECK_SIGILL(vuph(&v1, &v2, ES_64));
    CHECK_SIGILL(vuplh(&v1, &v2, ES_64));
    CHECK_SIGILL(vupl(&v1, &v2, ES_64));
    CHECK_SIGILL(vupll(&v1, &v2, ES_64));
}
