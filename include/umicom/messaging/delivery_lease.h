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

#ifdef __cplusplus
}
#endif

#endif
