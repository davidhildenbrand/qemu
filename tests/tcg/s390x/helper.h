/*
 * QEMU TCG Tests - s390x helper functions
 *
 * Copyright (C) 2019 Red Hat Inc
 *
 * Authors:
 *   David Hildenbrand <david@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */
#ifndef TEST_TCG_S390X_HELPER_H
#define TEST_TCG_S390X_HELPER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef union S390Vector {
    __uint128_t v;
    uint64_t d[2]; /* doubleword */
    uint32_t w[4]; /* word */
    uint16_t h[8]; /* halfword */
    uint8_t b[16]; /* byte */
} S390Vector;

#define ES_8    0
#define ES_16   1
#define ES_32   2
#define ES_64   3
#define ES_128  4

static inline void check(const char *s, bool cond)
{
    if (!cond) {
        fprintf(stderr, "Check failed: %s\n", s);
        exit(-1);
    }
}

#endif /* TEST_TCG_S390X_HELPER_H */
