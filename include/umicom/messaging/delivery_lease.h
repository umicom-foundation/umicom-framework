/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/delivery_lease.h
 *
 * PURPOSE:
 *   Declare delivery leases that prevent two consumers from concurrently processing the same durable message.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_DELIVERY_LEASE_H
#define UMICOM_MESSAGING_DELIVERY_LEASE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryLeaseStore UmiDeliveryLeaseStore;

UmiStatus umi_delivery_lease_store_create(size_t capacity,
                                          UmiDeliveryLeaseStore **out_store);
void umi_delivery_lease_store_destroy(UmiDeliveryLeaseStore *store);
UmiStatus umi_delivery_lease_acquire(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner,
                                     uint64_t now_nanoseconds,
                                     uint64_t duration_nanoseconds);
UmiStatus umi_delivery_lease_renew(UmiDeliveryLeaseStore *store,
                                   uint64_t message_id,
                                   const char *owner,
                                   uint64_t now_nanoseconds,
                                   uint64_t duration_nanoseconds);
UmiStatus umi_delivery_lease_release(UmiDeliveryLeaseStore *store,
                                     uint64_t message_id,
                                     const char *owner);
int umi_delivery_lease_is_owned(const UmiDeliveryLeaseStore *store,
                                uint64_t message_id,
                                const char *owner,
                                uint64_t now_nanoseconds);

#ifdef __cplusplus
}
#endif

#endif
