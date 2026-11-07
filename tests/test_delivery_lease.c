/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_lease.c
 *
 * PURPOSE:
 *   Verify exclusive delivery-lease acquisition, renewal, expiry takeover and release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

/* Rejected requests cannot consume capacity or shorten a valid lease. */
static void test_deadlines_and_identity(void)
{
    UmiDeliveryLeaseStore *store = NULL;
    char owner[UMI_DELIVERY_LEASE_OWNER_CAPACITY + 1U];
    memset(owner, 'w', sizeof(owner));
    owner[sizeof(owner) - 1U] = '\0';
    assert(umi_delivery_lease_store_create(1U, &store) == UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire(store, 1U, owner, 0U, 1U) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_delivery_lease_acquire(store, 1U, "a", UINT64_MAX, 1U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire(store, 1U, "a", UINT64_MAX - 1U, 2U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire(store, 1U, "a", 0U, 0U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire(store, 1U, "a", 10U, 20U) == UMI_STATUS_OK);
    assert(umi_delivery_lease_renew(store, 1U, "a", 15U, 0U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_renew(store, 1U, "a", 15U, UINT64_MAX) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_acquire(store, 1U, "a", 15U, UINT64_MAX) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_delivery_lease_is_owned(store, 1U, "a", 29U));
    assert(!umi_delivery_lease_is_owned(store, 1U, "a", 30U));
    assert(umi_delivery_lease_acquire(store, 2U, "b", 29U, 10U) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_delivery_lease_renew(store, 1U, "a", 30U, 5U) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_acquire(store, 2U, "b", 30U, 10U) == UMI_STATUS_OK);
    assert(!umi_delivery_lease_is_owned(store, 1U, "a", 30U));
    assert(umi_delivery_lease_release(store, 2U, "a") == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_delivery_lease_is_owned(store, 2U, "b", 31U));
    assert(umi_delivery_lease_release(store, 2U, "b") == UMI_STATUS_OK);

    owner[UMI_DELIVERY_LEASE_OWNER_CAPACITY - 1U] = '\0';
    assert(umi_delivery_lease_acquire(store, 3U, owner, UINT64_MAX - 2U, 2U) == UMI_STATUS_OK);
    assert(umi_delivery_lease_is_owned(store, 3U, owner, UINT64_MAX - 1U));
    assert(!umi_delivery_lease_is_owned(store, 3U, owner, UINT64_MAX));
    owner[UMI_DELIVERY_LEASE_OWNER_CAPACITY - 1U] = 'w';
    assert(!umi_delivery_lease_is_owned(store, 3U, owner, UINT64_MAX - 1U));
    assert(umi_delivery_lease_renew(store, 3U, owner, UINT64_MAX - 1U, 1U) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_delivery_lease_release(store, 3U, owner) == UMI_STATUS_CAPACITY_EXCEEDED);
    owner[UMI_DELIVERY_LEASE_OWNER_CAPACITY - 1U] = '\0';
    assert(umi_delivery_lease_release(store, 3U, owner) == UMI_STATUS_OK);
    umi_delivery_lease_store_destroy(store);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    test_deadlines_and_identity();
    UmiDeliveryLeaseStore *store = NULL;
    assert(umi_delivery_lease_store_create(4U, &store) == UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire(store, 11U, "worker-a",
                                      100U, 50U) == UMI_STATUS_OK);
    assert(umi_delivery_lease_acquire(store, 11U, "worker-b",
                                      120U, 50U) == UMI_STATUS_BUSY);
    assert(umi_delivery_lease_is_owned(store, 11U, "worker-a", 120U));
    assert(umi_delivery_lease_renew(store, 11U, "worker-a",
                                    120U, 50U) == UMI_STATUS_OK);
    assert(umi_delivery_lease_release(store, 11U, "worker-a") == UMI_STATUS_OK);
    umi_delivery_lease_store_destroy(store);
    return 0;
}
