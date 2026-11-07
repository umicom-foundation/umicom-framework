/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/delivery_lease.h
 *
 * PURPOSE:
 *   Declare delivery leases that prevent two consumers from concurrently processing the same durable message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_DELIVERY_LEASE_H
#define UMICOM_MESSAGING_DELIVERY_LEASE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#define UMI_DELIVERY_LEASE_OWNER_CAPACITY 128U

/* These leases coordinate serial calls to one in-memory store. They are not a
 * distributed lock, durable record, or fencing mechanism: expiry cannot stop
 * an old worker from performing an external side effect. Applications must
 * separately enforce durable ownership before committing that effect.
 * Owner names are readable, nonempty C strings of at most 127 bytes.
 * Oversized owners return CAPACITY_EXCEEDED; acquire/renew reject zero duration
 * and any deadline that cannot be represented in uint64_t. Rejected operations
 * preserve the existing lease. Expiry is exclusive: at the deadline the lease
 * is no longer owned. All callers must use the same monotonic clock domain. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the delivery lease store data shared with callers of this public contract.
 */
typedef struct UmiDeliveryLeaseStore UmiDeliveryLeaseStore;

/**
 * Initialise delivery lease store from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_delivery_lease_store_create(size_t capacity,
                                          UmiDeliveryLeaseStore **out_store);
/**
 * Release or reset state held by delivery lease store so the same storage can be reused
 * safely.
 */
void umi_delivery_lease_store_destroy(UmiDeliveryLeaseStore *store);
/**
 * Provide the delivery lease acquire operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_lease_acquire(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner,
                                     uint64_t now_nanoseconds,
                                     uint64_t duration_nanoseconds);
/**
 * Provide the delivery lease renew operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_lease_renew(UmiDeliveryLeaseStore *store,
                                   uint64_t message_id,
                                   const char *owner,
                                   uint64_t now_nanoseconds,
                                   uint64_t duration_nanoseconds);
/**
 * Release or reset state held by delivery lease so the same storage can be reused safely.
 */
UmiStatus umi_delivery_lease_release(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner);
/**
 * Provide the delivery lease is owned operation used by this module and its client
 * applications.
 */
int umi_delivery_lease_is_owned(const UmiDeliveryLeaseStore *store,
                                uint64_t message_id,
                                const char *owner,
                                uint64_t now_nanoseconds);


/*
 * A ticket identifies one successful acquisition in the issuing store's
 * lifetime. Keep the ticket with the work item, not in a shared "latest ticket"
 * variable. Copies remain valid only for that acquisition. These public values
 * are correlation data, not secrets or authentication credentials. Never use a
 * ticket with another store or retain it after destroying its store.
 */
typedef struct UmiDeliveryLeaseTicket {
    uint64_t message_id;
    uint64_t generation;
} UmiDeliveryLeaseTicket;

/*
 * Acquire a new generation. Any unexpired lease returns BUSY, including one
 * bearing the same owner name. Successful renewal uses renew_ticket instead.
 * On failure, the output ticket and store remain unchanged. Generations never
 * wrap or repeat in one store; exhaustion returns CAPACITY_EXCEEDED.
 * out_ticket points to writable storage distinct from the owner string.
 */
UmiStatus umi_delivery_lease_acquire_ticket(UmiDeliveryLeaseStore *store,
                                           uint64_t message_id,
                                           const char *owner,
                                           uint64_t now_nanoseconds,
                                           uint64_t duration_nanoseconds,
                                           UmiDeliveryLeaseTicket *out_ticket);

/* Renew only the exact, unexpired acquisition; retain its generation. */
UmiStatus umi_delivery_lease_renew_ticket(UmiDeliveryLeaseStore *store,
                                         UmiDeliveryLeaseTicket ticket,
                                         const char *owner,
                                         uint64_t now_nanoseconds,
                                         uint64_t duration_nanoseconds);

/*
 * Release only the exact acquisition. Releasing an expired ticket is allowed
 * if it still identifies the stored record; it never releases a replacement.
 * Legacy renew/release calls refuse ticket-protected records.
 */
UmiStatus umi_delivery_lease_release_ticket(UmiDeliveryLeaseStore *store,
                                           UmiDeliveryLeaseTicket ticket,
                                           const char *owner);

/* Observe ownership at a supplied monotonic time; this does not reserve work. */
int umi_delivery_lease_ticket_is_owned(const UmiDeliveryLeaseStore *store,
                                       UmiDeliveryLeaseTicket ticket,
                                       const char *owner,
                                       uint64_t now_nanoseconds);

#ifdef __cplusplus
}
#endif

#endif
