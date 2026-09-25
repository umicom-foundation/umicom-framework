/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/position_store.c
 *
 * PURPOSE:
 *   Implement monotonic broker position snapshot retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/position_store.h"

#include <string.h>

void umi_broker_position_store_init(UmiBrokerPositionStore *store)
{
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

const UmiBrokerPositionSnapshot *umi_broker_position_store_find(
    const UmiBrokerPositionStore *store,
    const char *accountId,
    const char *instrumentId)
{
    size_t index;
    if (store == NULL || accountId == NULL || instrumentId == NULL) return NULL;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->positions[index].accountId, accountId) == 0 &&
            strcmp(store->positions[index].instrumentId, instrumentId) == 0) {
            return &store->positions[index];
        }
    }
    return NULL;
}

UmiStatus umi_broker_position_store_upsert(
    UmiBrokerPositionStore *store,
    const UmiBrokerPositionSnapshot *position)
{
    size_t index;

    if (store == NULL || position == NULL ||
        position->accountId[0] == '\0' ||
        position->instrumentId[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < store->count; ++index) {
        UmiBrokerPositionSnapshot *current = &store->positions[index];
        if (strcmp(current->accountId, position->accountId) == 0 &&
            strcmp(current->instrumentId, position->instrumentId) == 0) {
            if (position->providerSequence < current->providerSequence) {
                return UMI_STATUS_INVALID_STATE;
            }
            *current = *position;
            store->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    if (store->count >= UMI_BROKER_POSITION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    store->positions[store->count++] = *position;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
