/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/account_store.h
 *
 * PURPOSE:
 *   Retain bounded broker account snapshots for reconciliation and UI projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_ACCOUNT_STORE_H
#define UMICOM_BROKER_CONNECTIVITY_ACCOUNT_STORE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_ACCOUNT_CAPACITY 32U

typedef struct UmiBrokerAccountSnapshot {
    char accountId[64];
    char baseCurrency[16];
    double netLiquidation;
    double cash;
    double buyingPower;
    double marginUsed;
    double marginAvailable;
    double dailyPnl;
    double unrealisedPnl;
    double realisedPnl;
    uint64_t providerSequence;
    uint64_t eventTimeMilliseconds;
} UmiBrokerAccountSnapshot;

typedef struct UmiBrokerAccountStore {
    UmiBrokerAccountSnapshot accounts[UMI_BROKER_ACCOUNT_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiBrokerAccountStore;

void umi_broker_account_store_init(UmiBrokerAccountStore *store);
UmiStatus umi_broker_account_store_upsert(
    UmiBrokerAccountStore *store,
    const UmiBrokerAccountSnapshot *snapshot);
const UmiBrokerAccountSnapshot *umi_broker_account_store_find(
    const UmiBrokerAccountStore *store,
    const char *accountId);

#ifdef __cplusplus
}
#endif
#endif
