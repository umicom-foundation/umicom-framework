/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/delivery_lease.c
 *
 * PURPOSE:
 *   Implement bounded delivery-lease acquisition, renewal, ownership checks, expiry replacement and release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/delivery_lease.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiDeliveryLease {
    uint64_t message_id;
    uint64_t expires_at;
    char owner[128];
    int used;
} UmiDeliveryLease;

struct UmiDeliveryLeaseStore {
    UmiDeliveryLease *leases;
    size_t capacity;
};

UmiStatus umi_delivery_lease_store_create(size_t capacity,
                                          UmiDeliveryLeaseStore **out_store)
{
    UmiDeliveryLeaseStore *store;
    if (capacity == 0U || out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiDeliveryLeaseStore *)calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    store->leases = (UmiDeliveryLease *)calloc(capacity,
                                               sizeof(*store->leases));
    if (store->leases == NULL) {
        free(store);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    store->capacity = capacity;
    *out_store = store;
    return UMI_STATUS_OK;
}

void umi_delivery_lease_store_destroy(UmiDeliveryLeaseStore *store)
{
    if (store == NULL) return;
    free(store->leases);
    free(store);
}

static UmiDeliveryLease *find_lease(UmiDeliveryLeaseStore *store,
                                    uint64_t message_id)
{
    size_t index;
    for (index = 0U; index < store->capacity; ++index) {
        if (store->leases[index].used &&
            store->leases[index].message_id == message_id) {
            return &store->leases[index];
        }
    }
    return NULL;
}

UmiStatus umi_delivery_lease_acquire(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner,
                                     uint64_t now_nanoseconds,
                                     uint64_t duration_nanoseconds)
{
    size_t index;
    UmiDeliveryLease *lease;
    if (store == NULL || message_id == 0U || owner == NULL ||
        owner[0] == '\0' || duration_nanoseconds == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    lease = find_lease(store, message_id);
    if (lease != NULL && lease->expires_at > now_nanoseconds &&
        strcmp(lease->owner, owner) != 0) {
        return UMI_STATUS_BUSY;
    }
    if (lease == NULL) {
        for (index = 0U; index < store->capacity; ++index) {
            if (!store->leases[index].used ||
                store->leases[index].expires_at <= now_nanoseconds) {
                lease = &store->leases[index];
                break;
            }
        }
    }
    if (lease == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    lease->used = 1;
    lease->message_id = message_id;
    lease->expires_at = now_nanoseconds + duration_nanoseconds;
    (void)snprintf(lease->owner, sizeof(lease->owner), "%s", owner);
    return UMI_STATUS_OK;
}

UmiStatus umi_delivery_lease_renew(UmiDeliveryLeaseStore *store,
                                   uint64_t message_id,
                                   const char *owner,
                                   uint64_t now_nanoseconds,
                                   uint64_t duration_nanoseconds)
{
    UmiDeliveryLease *lease;
    if (store == NULL || owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    lease = find_lease(store, message_id);
    if (lease == NULL) return UMI_STATUS_NOT_FOUND;
    if (strcmp(lease->owner, owner) != 0 ||
        lease->expires_at <= now_nanoseconds) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    lease->expires_at = now_nanoseconds + duration_nanoseconds;
    return UMI_STATUS_OK;
}

UmiStatus umi_delivery_lease_release(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner)
{
    UmiDeliveryLease *lease;
    if (store == NULL || owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    lease = find_lease(store, message_id);
    if (lease == NULL) return UMI_STATUS_NOT_FOUND;
    if (strcmp(lease->owner, owner) != 0) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    (void)memset(lease, 0, sizeof(*lease));
    return UMI_STATUS_OK;
}

int umi_delivery_lease_is_owned(const UmiDeliveryLeaseStore *store,
                                uint64_t message_id,
                                const char *owner,
                                uint64_t now_nanoseconds)
{
    size_t index;
    if (store == NULL || owner == NULL) return 0;
    for (index = 0U; index < store->capacity; ++index) {
        const UmiDeliveryLease *lease = &store->leases[index];
        if (lease->used && lease->message_id == message_id &&
            lease->expires_at > now_nanoseconds &&
            strcmp(lease->owner, owner) == 0) {
            return 1;
        }
    }
    return 0;
}
