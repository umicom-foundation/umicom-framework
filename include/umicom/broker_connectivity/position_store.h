/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/position_store.h
 *
 * PURPOSE:
 *   Retain bounded broker position snapshots for explicit reconciliation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_POSITION_STORE_H
#define UMICOM_BROKER_CONNECTIVITY_POSITION_STORE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_POSITION_CAPACITY 256U

typedef struct UmiBrokerPositionSnapshot {
    char accountId[64];
    char instrumentId[64];
    char symbol[32];
    double quantity;
    double averageCost;
    double marketPrice;
    double unrealisedPnl;
    double realisedPnl;
    uint64_t providerSequence;
    uint64_t eventTimeMilliseconds;
} UmiBrokerPositionSnapshot;

typedef struct UmiBrokerPositionStore {
    UmiBrokerPositionSnapshot positions[UMI_BROKER_POSITION_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiBrokerPositionStore;

void umi_broker_position_store_init(UmiBrokerPositionStore *store);
UmiStatus umi_broker_position_store_upsert(
    UmiBrokerPositionStore *store,
    const UmiBrokerPositionSnapshot *position);
const UmiBrokerPositionSnapshot *umi_broker_position_store_find(
    const UmiBrokerPositionStore *store,
    const char *accountId,
    const char *instrumentId);

#ifdef __cplusplus
}
#endif
#endif
