/*
 * QEMU TCG Tests - s390x VECTOR LOAD AND REPLICATE
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

static inline void vlrep(S390Vector *v1, const void *a2, uint8_t m3)
{
    asm volatile("vlrep %[v1], 0(%[a2]), %[m3]\n"
                 : [v1] "+v" (v1->v)
                 : [a2] "d" (a2),
                   [m3] "i" (m3)
                 : "memory");
}

int main(void)
{
    const uint64_t data = 0x0123456789abcdefull;
    S390Vector v1 = {};

    vlrep(&v1, &data, ES_8);
    check("8", v1.d[0] == 0x0101010101010101ull &&
               v1.d[1] == 0x0101010101010101ull);
    vlrep(&v1, &data, ES_16);
    check("16", v1.d[0] == 0x0123012301230123ull &&
                v1.d[1] == 0x0123012301230123ull);
    vlrep(&v1, &data, ES_32);
    check("32", v1.d[0] == 0x0123456701234567ull &&
                v1.d[1] == 0x0123456701234567ull);
    vlrep(&v1, &data, ES_64);
    check("64", v1.d[0] == 0x0123456789abcdefull &&
                v1.d[1] == 0x0123456789abcdefull);
    CHECK_SIGILL(vlrep(&v1, &data, ES_128));
    return 0;
}
