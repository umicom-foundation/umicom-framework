/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_lease.c
 *
 * PURPOSE:
 *   Verify exclusive delivery-lease acquisition, renewal, expiry takeover and release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

int main(void)
{
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
