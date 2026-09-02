/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_lock.c
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

#include "umicom/delivery/release_lock.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise release lock from caller-provided values so later operations receive a known
 * state.
 */
void umi_release_lock_init(UmiReleaseLock *lock)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lock != NULL) (void)memset(lock, 0, sizeof(*lock));
}
/*
 * Provide the release lock acquire operation used by this module and its client
 * applications.
 */
UmiStatus umi_release_lock_acquire(UmiReleaseLock *lock,
                                   const char *owner_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lock == NULL || owner_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (lock->held) return UMI_STATUS_BUSY;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(lock->owner_id, sizeof(lock->owner_id), owner_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    lock->held = 1;
    return UMI_STATUS_OK;
}
/* Release or reset state held by release lock so the same storage can be reused safely. */
UmiStatus umi_release_lock_release(UmiReleaseLock *lock,
                                   const char *owner_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lock == NULL || owner_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!lock->held || strcmp(lock->owner_id, owner_id) != 0) return UMI_STATUS_PERMISSION_DENIED;
    lock->held = 0;
    lock->owner_id[0] = '\0';
    return UMI_STATUS_OK;
}
