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

/**
 * Provide the message strdup operation used by this module and its client applications.
 */
static inline char *umi_message_strdup(const char *value)
{
    size_t length;
    char *copy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) value = "";
    length = strlen(value);
    copy = (char *)malloc(length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return NULL;
    (void)memcpy(copy, value, length + 1U);
    return copy;
}

/**
 * Provide the message memdup operation used by this module and its client applications.
 */
static inline void *umi_message_memdup(const void *value, size_t size)
{
    void *copy;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (size == 0U) return NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return NULL;
    copy = malloc(size);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return NULL;
    (void)memcpy(copy, value, size);
    return copy;
}

/**
 * Represent the spin lock data shared with callers of this public contract.
 */
typedef struct UmiSpinLock {
    atomic_flag flag;
} UmiSpinLock;

/**
 * Initialise spin from caller-provided values so later operations receive a known state.
 */
static inline void umi_spin_init(UmiSpinLock *lock)
{
    atomic_flag_clear(&lock->flag);
}

/**
 * Provide the spin lock operation used by this module and its client applications.
 */
static inline void umi_spin_lock(UmiSpinLock *lock)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (atomic_flag_test_and_set_explicit(&lock->flag,
                                              memory_order_acquire)) {
    }
}

/**
 * Provide the spin unlock operation used by this module and its client applications.
 */
static inline void umi_spin_unlock(UmiSpinLock *lock)
{
    atomic_flag_clear_explicit(&lock->flag, memory_order_release);
}

#endif
