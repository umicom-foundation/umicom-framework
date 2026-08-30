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

/* BEGINNER NOTE:
 * Only one operation should mutate release state at a time; this reference lock makes that rule explicit even before an external coordinator is used.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_RELEASE_LOCK_H
#define INCLUDE_UMICOM_DELIVERY_RELEASE_LOCK_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseLock {
    char owner_id[UMI_DELIVERY_ID_CAPACITY];
    int held;
} UmiReleaseLock;
void umi_release_lock_init(UmiReleaseLock *lock);
UmiStatus umi_release_lock_acquire(UmiReleaseLock *lock,
                                   const char *owner_id);
UmiStatus umi_release_lock_release(UmiReleaseLock *lock,
                                   const char *owner_id);

#ifdef __cplusplus
}
#endif

#endif
