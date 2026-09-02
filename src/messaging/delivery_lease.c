/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/delivery_lease.c
 *
 * PURPOSE:
 *   Implement bounded delivery-lease acquisition, renewal, ownership checks, expiry replacement and release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Initialise delivery lease store from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_delivery_lease_store_create(size_t capacity,
                                          UmiDeliveryLeaseStore **out_store)
{
    UmiDeliveryLeaseStore *store;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiDeliveryLeaseStore *)calloc(1U, sizeof(*store));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    store->leases = (UmiDeliveryLease *)calloc(capacity,
                                               sizeof(*store->leases));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store->leases == NULL) {
        free(store);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    store->capacity = capacity;
    *out_store = store;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by delivery lease store so the same storage can be reused
 * safely.
 */
void umi_delivery_lease_store_destroy(UmiDeliveryLeaseStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    free(store->leases);
    free(store);
}

/* Provide the find lease operation used by this module and its client applications. */
static UmiDeliveryLease *find_lease(UmiDeliveryLeaseStore *store,
                                    uint64_t message_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->capacity; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (store->leases[index].used &&
            store->leases[index].message_id == message_id) {
            return &store->leases[index];
        }
    }
    return NULL;
}

/*
 * Provide the delivery lease acquire operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_lease_acquire(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner,
                                     uint64_t now_nanoseconds,
                                     uint64_t duration_nanoseconds)
{
    size_t index;
    UmiDeliveryLease *lease;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || message_id == 0U || owner == NULL ||
        owner[0] == '\0' || duration_nanoseconds == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    lease = find_lease(store, message_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lease != NULL && lease->expires_at > now_nanoseconds &&
        strcmp(lease->owner, owner) != 0) {
        return UMI_STATUS_BUSY;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lease == NULL) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < store->capacity; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!store->leases[index].used ||
                store->leases[index].expires_at <= now_nanoseconds) {
                lease = &store->leases[index];
                break;
            }
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lease == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    lease->used = 1;
    lease->message_id = message_id;
    lease->expires_at = now_nanoseconds + duration_nanoseconds;
    (void)snprintf(lease->owner, sizeof(lease->owner), "%s", owner);
    return UMI_STATUS_OK;
}

/*
 * Provide the delivery lease renew operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_lease_renew(UmiDeliveryLeaseStore *store,
                                   uint64_t message_id,
                                   const char *owner,
                                   uint64_t now_nanoseconds,
                                   uint64_t duration_nanoseconds)
{
    UmiDeliveryLease *lease;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    lease = find_lease(store, message_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lease == NULL) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(lease->owner, owner) != 0 ||
        lease->expires_at <= now_nanoseconds) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    lease->expires_at = now_nanoseconds + duration_nanoseconds;
    return UMI_STATUS_OK;
}

/* Release or reset state held by delivery lease so the same storage can be reused safely. */
UmiStatus umi_delivery_lease_release(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner)
{
    UmiDeliveryLease *lease;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    lease = find_lease(store, message_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lease == NULL) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(lease->owner, owner) != 0) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    (void)memset(lease, 0, sizeof(*lease));
    return UMI_STATUS_OK;
}

/*
 * Provide the delivery lease is owned operation used by this module and its client
 * applications.
 */
int umi_delivery_lease_is_owned(const UmiDeliveryLeaseStore *store,
                                uint64_t message_id,
                                const char *owner,
                                uint64_t now_nanoseconds)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || owner == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->capacity; ++index) {
        const UmiDeliveryLease *lease = &store->leases[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (lease->used && lease->message_id == message_id &&
            lease->expires_at > now_nanoseconds &&
            strcmp(lease->owner, owner) == 0) {
            return 1;
        }
    }
    return 0;
}
