/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_lock.h
 *
 * PURPOSE:
 *   Provide a small in-memory release lock used to prevent concurrent promotion operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Only one operation should mutate release state at a time; this reference lock makes that rule explicit even before an external coordinator is used.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_RELEASE_LOCK_H
#define INCLUDE_UMICOM_DELIVERY_RELEASE_LOCK_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the release lock data shared with callers of this public contract.
 */
typedef struct UmiReleaseLock {
    char owner_id[UMI_DELIVERY_ID_CAPACITY];
    int held;
} UmiReleaseLock;
/**
 * Initialise release lock from caller-provided values so later operations receive a known
 * state.
 */
void umi_release_lock_init(UmiReleaseLock *lock);
/**
 * Provide the release lock acquire operation used by this module and its client
 * applications.
 */
UmiStatus umi_release_lock_acquire(UmiReleaseLock *lock,
                                   const char *owner_id);
/**
 * Release or reset state held by release lock so the same storage can be reused safely.
 */
UmiStatus umi_release_lock_release(UmiReleaseLock *lock,
                                   const char *owner_id);

#ifdef __cplusplus
}
#endif

#endif
