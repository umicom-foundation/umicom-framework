/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/include/messaging_internal.h
 *
 * PURPOSE:
 *   Provide private allocation, copy, comparison and spin-lock helpers shared by Integration Fabric implementations without exposing implementation details publicly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_INTERNAL_H
#define UMICOM_MESSAGING_INTERNAL_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline char *umi_message_strdup(const char *value)
{
    size_t length;
    char *copy;
    if (value == NULL) value = "";
    length = strlen(value);
    copy = (char *)malloc(length + 1U);
    if (copy == NULL) return NULL;
    (void)memcpy(copy, value, length + 1U);
    return copy;
}

static inline void *umi_message_memdup(const void *value, size_t size)
{
    void *copy;
    if (size == 0U) return NULL;
    if (value == NULL) return NULL;
    copy = malloc(size);
    if (copy == NULL) return NULL;
    (void)memcpy(copy, value, size);
    return copy;
}

typedef struct UmiSpinLock {
    atomic_flag flag;
} UmiSpinLock;

static inline void umi_spin_init(UmiSpinLock *lock)
{
    atomic_flag_clear(&lock->flag);
}

static inline void umi_spin_lock(UmiSpinLock *lock)
{
    while (atomic_flag_test_and_set_explicit(&lock->flag,
                                              memory_order_acquire)) {
    }
}

static inline void umi_spin_unlock(UmiSpinLock *lock)
{
    atomic_flag_clear_explicit(&lock->flag, memory_order_release);
}

#endif
