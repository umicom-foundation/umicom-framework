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
    /* Zero denotes a legacy lease; positive generations protect ticketed work. */
    uint64_t generation;
    /* Public capacity keeps callers and the stored ownership token aligned.
     * Previous storage declaration retained for review. */
#if 0
    char owner[128];
#else
    char owner[UMI_DELIVERY_LEASE_OWNER_CAPACITY];
#endif
    int used;
} UmiDeliveryLease;

struct UmiDeliveryLeaseStore {
    UmiDeliveryLease *leases;
    size_t capacity;
    /* Store-wide sequence survives slot recycling and release. */
    uint64_t last_generation;
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

/* Exact owner names prevent truncation from changing who can renew or release
 * a lease. The API accepts readable C strings, never arbitrary network buffers. */
static UmiStatus validate_owner(const char *owner)
{
    size_t length = 0U;
    if (owner == NULL || owner[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    while (length < UMI_DELIVERY_LEASE_OWNER_CAPACITY && owner[length] != '\0')
        ++length;
    return length == UMI_DELIVERY_LEASE_OWNER_CAPACITY
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/* Validate addition before any lease mutation. Wraparound would make a new
 * lease appear expired, allowing another owner to take the same message. */
static int valid_deadline(uint64_t now, uint64_t duration)
{
    return duration != 0U && duration <= UINT64_MAX - now;
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
/* Unchecked deadline addition and truncated owner names could invalidate lease ownership. Checked acquisition below replaces them. Retained for implementation review. */
#if 0
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
#endif

/* Owner names alone cannot identify a restarted acquisition. The shared
 * implementation below adds ticket protection while preserving the former
 * checked acquisition here for review. */
#if 0
UmiStatus umi_delivery_lease_acquire(UmiDeliveryLeaseStore *store,
                                   uint64_t message_id,
                                   const char *owner,
                                   uint64_t now_nanoseconds,
                                   uint64_t duration_nanoseconds)
{
    size_t index;
    UmiDeliveryLease *lease;
    UmiStatus status;
    if (store == NULL || message_id == 0U ||
        !valid_deadline(now_nanoseconds, duration_nanoseconds))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_owner(owner);
    if (status != UMI_STATUS_OK) return status;
    lease = find_lease(store, message_id);
    if (lease != NULL && lease->expires_at > now_nanoseconds &&
        strcmp(lease->owner, owner) != 0) return UMI_STATUS_BUSY;
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
    lease->message_id = message_id;
    lease->expires_at = now_nanoseconds + duration_nanoseconds;
    memcpy(lease->owner, owner, strlen(owner) + 1U);
    lease->used = 1;
    return UMI_STATUS_OK;
}

#endif

/*
 * Centralise slot selection and publication for both APIs. An acquisition with
 * a ticket always starts a distinct generation. Legacy callers retain their
 * same-owner renewal behaviour only for legacy records; they cannot bypass
 * ticket identity on a protected record.
 */
static UmiStatus acquire_lease(UmiDeliveryLeaseStore *store,
                               uint64_t message_id, const char *owner,
                               uint64_t now, uint64_t duration,
                               UmiDeliveryLeaseTicket *out_ticket)
{
    size_t index;
    UmiDeliveryLease *lease;
    UmiStatus status;
    uint64_t generation = 0U;
    if (store == NULL || message_id == 0U || !valid_deadline(now, duration))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_owner(owner);
    if (status != UMI_STATUS_OK) return status;
    lease = find_lease(store, message_id);
    if (lease != NULL && lease->expires_at > now &&
        (out_ticket != NULL || lease->generation != 0U ||
         strcmp(lease->owner, owner) != 0))
        return UMI_STATUS_BUSY;
    if (lease == NULL) {
        for (index = 0U; index < store->capacity; ++index) {
            if (!store->leases[index].used ||
                store->leases[index].expires_at <= now) {
                lease = &store->leases[index];
                break;
            }
        }
    }
    if (lease == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (out_ticket != NULL) {
        if (store->last_generation == UINT64_MAX)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        generation = store->last_generation + 1U;
    }

    /* Validation is complete before replacing any record or publishing output. */
    lease->message_id = message_id;
    lease->expires_at = now + duration;
    memcpy(lease->owner, owner, strlen(owner) + 1U);
    lease->generation = generation;
    lease->used = 1;
    if (out_ticket != NULL) {
        store->last_generation = generation;
        out_ticket->message_id = message_id;
        out_ticket->generation = generation;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_delivery_lease_acquire(UmiDeliveryLeaseStore *store,
                                   uint64_t message_id,
                                   const char *owner,
                                   uint64_t now_nanoseconds,
                                   uint64_t duration_nanoseconds)
{
    return acquire_lease(store, message_id, owner, now_nanoseconds,
                         duration_nanoseconds, NULL);
}

UmiStatus umi_delivery_lease_acquire_ticket(UmiDeliveryLeaseStore *store,
                                           uint64_t message_id,
                                           const char *owner,
                                           uint64_t now_nanoseconds,
                                           uint64_t duration_nanoseconds,
                                           UmiDeliveryLeaseTicket *out_ticket)
{
    if (out_ticket == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return acquire_lease(store, message_id, owner, now_nanoseconds,
                         duration_nanoseconds, out_ticket);
}

/*
 * Provide the delivery lease renew operation used by this module and its client
 * applications.
 */
/* A zero or overflowing renewal could expire a valid lease. Validation below preserves the previous deadline when rejected. Retained for implementation review. */
#if 0
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
#endif

UmiStatus umi_delivery_lease_renew(UmiDeliveryLeaseStore *store,
                                 uint64_t message_id,
                                 const char *owner,
                                 uint64_t now_nanoseconds,
                                 uint64_t duration_nanoseconds)
{
    UmiDeliveryLease *lease;
    UmiStatus status;
    if (store == NULL || message_id == 0U ||
        !valid_deadline(now_nanoseconds, duration_nanoseconds))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_owner(owner);
    if (status != UMI_STATUS_OK) return status;
    lease = find_lease(store, message_id);
    if (lease == NULL) return UMI_STATUS_NOT_FOUND;
    /* Ticket-protected records must be renewed through the ticket API.
     * The former name-only guard is retained for review. */
#if 0
    if (strcmp(lease->owner, owner) != 0 || lease->expires_at <= now_nanoseconds)
        return UMI_STATUS_PERMISSION_DENIED;
#else
    if (lease->generation != 0U || strcmp(lease->owner, owner) != 0 ||
        lease->expires_at <= now_nanoseconds)
        return UMI_STATUS_PERMISSION_DENIED;
#endif
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
    /* Owner validation is shared with acquire and renew; retain the previous
     * permissive guard so the contract change remains reviewable. */
#if 0
    if (store == NULL || owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
#else
    if (store == NULL || message_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    {
        UmiStatus status = validate_owner(owner);
        if (status != UMI_STATUS_OK) return status;
    }
#endif
    lease = find_lease(store, message_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lease == NULL) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    /* Name-only release cannot retire a newer ticketed acquisition.
     * Preserve the earlier guard for implementation review. */
#if 0
    if (strcmp(lease->owner, owner) != 0) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
#else
    if (lease->generation != 0U || strcmp(lease->owner, owner) != 0)
        return UMI_STATUS_PERMISSION_DENIED;
#endif
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
    /* Invalid identities cannot own a lease; the prior guard is retained. */
#if 0
    if (store == NULL || owner == NULL) return 0;
#else
    if (store == NULL || message_id == 0U || validate_owner(owner) != UMI_STATUS_OK)
        return 0;
#endif
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


/* Resolve the exact acquisition without interpreting public ticket values as
 * security credentials. All callers serialize access to the store. */
static UmiStatus resolve_ticket(UmiDeliveryLeaseStore *store,
                                UmiDeliveryLeaseTicket ticket,
                                const char *owner,
                                UmiDeliveryLease **out_lease)
{
    UmiStatus status;
    UmiDeliveryLease *lease;
    if (store == NULL || ticket.message_id == 0U || ticket.generation == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_owner(owner);
    if (status != UMI_STATUS_OK) return status;
    lease = find_lease(store, ticket.message_id);
    if (lease == NULL) return UMI_STATUS_NOT_FOUND;
    if (lease->generation != ticket.generation ||
        strcmp(lease->owner, owner) != 0)
        return UMI_STATUS_PERMISSION_DENIED;
    *out_lease = lease;
    return UMI_STATUS_OK;
}

UmiStatus umi_delivery_lease_renew_ticket(UmiDeliveryLeaseStore *store,
                                         UmiDeliveryLeaseTicket ticket,
                                         const char *owner,
                                         uint64_t now_nanoseconds,
                                         uint64_t duration_nanoseconds)
{
    UmiDeliveryLease *lease = NULL;
    UmiStatus status;
    if (!valid_deadline(now_nanoseconds, duration_nanoseconds))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = resolve_ticket(store, ticket, owner, &lease);
    if (status != UMI_STATUS_OK) return status;
    if (lease->expires_at <= now_nanoseconds) return UMI_STATUS_PERMISSION_DENIED;
    lease->expires_at = now_nanoseconds + duration_nanoseconds;
    return UMI_STATUS_OK;
}

UmiStatus umi_delivery_lease_release_ticket(UmiDeliveryLeaseStore *store,
                                           UmiDeliveryLeaseTicket ticket,
                                           const char *owner)
{
    UmiDeliveryLease *lease = NULL;
    UmiStatus status = resolve_ticket(store, ticket, owner, &lease);
    if (status != UMI_STATUS_OK) return status;
    memset(lease, 0, sizeof(*lease));
    return UMI_STATUS_OK;
}

int umi_delivery_lease_ticket_is_owned(const UmiDeliveryLeaseStore *store,
                                       UmiDeliveryLeaseTicket ticket,
                                       const char *owner,
                                       uint64_t now_nanoseconds)
{
    size_t index;
    if (store == NULL || ticket.message_id == 0U || ticket.generation == 0U ||
        validate_owner(owner) != UMI_STATUS_OK)
        return 0;
    for (index = 0U; index < store->capacity; ++index) {
        const UmiDeliveryLease *lease = &store->leases[index];
        if (lease->used && lease->message_id == ticket.message_id &&
            lease->generation == ticket.generation &&
            lease->expires_at > now_nanoseconds &&
            strcmp(lease->owner, owner) == 0)
            return 1;
    }
    return 0;
}
