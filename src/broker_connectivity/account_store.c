/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/account_store.c
 *
 * PURPOSE:
 *   Implement monotonic broker account snapshot retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/broker_connectivity/account_store.h"

#include <string.h>

void umi_broker_account_store_init(UmiBrokerAccountStore *store)
{
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

const UmiBrokerAccountSnapshot *umi_broker_account_store_find(
    const UmiBrokerAccountStore *store,
    const char *accountId)
{
    size_t index;
    if (store == NULL || accountId == NULL) return NULL;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->accounts[index].accountId, accountId) == 0) {
            return &store->accounts[index];
        }
    }
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_broker_account_store_upsert(
    UmiBrokerAccountStore *store,
    const UmiBrokerAccountSnapshot *snapshot)
{
    size_t index;

    if (store == NULL || snapshot == NULL || snapshot->accountId[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->accounts[index].accountId, snapshot->accountId) == 0) {
            if (snapshot->providerSequence <
                store->accounts[index].providerSequence) {
                return UMI_STATUS_INVALID_STATE;
            }
            store->accounts[index] = *snapshot;
            store->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    if (store->count >= UMI_BROKER_ACCOUNT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    store->accounts[store->count++] = *snapshot;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_broker_account_store_upsert(
    UmiBrokerAccountStore *store,
    const UmiBrokerAccountSnapshot *snapshot)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (store != NULL && store->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t index;

    if (store == NULL || snapshot == NULL || snapshot->accountId[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->accounts[index].accountId, snapshot->accountId) == 0) {
            if (snapshot->providerSequence <
                store->accounts[index].providerSequence) {
                return UMI_STATUS_INVALID_STATE;
            }
            store->accounts[index] = *snapshot;
            store->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    if (store->count >= UMI_BROKER_ACCOUNT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    store->accounts[store->count++] = *snapshot;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
