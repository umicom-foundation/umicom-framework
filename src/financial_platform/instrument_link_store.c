/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/instrument_link_store.c
 *
 * PURPOSE:
 *   Implement monotonic instrument/product/currency linkage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/instrument_link_store.h"
#include <string.h>

void umi_financial_instrument_link_store_init(UmiFinancialInstrumentLinkStore *store)
{
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

const UmiFinancialInstrumentLink *umi_financial_instrument_link_store_find(
    const UmiFinancialInstrumentLinkStore *store,
    const char *instrumentId)
{
    size_t i;
    if (store == NULL || instrumentId == NULL) return NULL;
    for (i = 0U; i < store->count; ++i)
        if (strcmp(store->links[i].instrumentId, instrumentId) == 0)
            return &store->links[i];
    return NULL;
}

UmiStatus umi_financial_instrument_link_store_upsert(
    UmiFinancialInstrumentLinkStore *store,
    const UmiFinancialInstrumentLink *link)
{
    size_t i;
    if (store == NULL || link == NULL || link->instrumentId[0] == '\0' ||
        link->productId[0] == '\0' || link->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (i = 0U; i < store->count; ++i) {
        if (strcmp(store->links[i].instrumentId, link->instrumentId) == 0) {
            if (link->version < store->links[i].version)
                return UMI_STATUS_INVALID_STATE;
            store->links[i] = *link;
            store->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (store->count >= UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    store->links[store->count++] = *link;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
