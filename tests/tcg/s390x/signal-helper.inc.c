/*
 * QEMU TCG Tests - s390x signal handling helper functions
 *
 * Copyright (C) 2019 Red Hat Inc
 *
 * Authors:
 *   David Hildenbrand <david@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include "helper.h"

jmp_buf jmp_env;

static inline void handle_sigill(int sig)
{
    if (sig != SIGILL) {
        check("Wrong signal received", false);
    }
    siglongjmp(jmp_env, 1);
}

#define CHECK_SIGILL(STATEMENT)                         \
do {                                                    \
    if (signal(SIGILL, handle_sigill) == SIG_ERR) {     \
        check("SIGILL not registered", false);          \
    }                                                   \
    if (sigsetjmp(jmp_env, 1) == 0) {                   \
        STATEMENT;                                      \
        check("SIGILL not triggered", false);           \
    }                                                   \
    if (signal(SIGILL, SIG_DFL) == SIG_ERR) {           \
        check("SIGILL not registered", false);          \
    }                                                   \
} while (0)
