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
#include <stdint.h>
#include "umicom/financial_platform/instrument_link_store.h"
#include <string.h>

#include "control_query.h"

/* Retained for review: the former owner trusted public array bounds and inline
 * identities. Framework now admits every active row before lookup or mutation,
 * retains existing lifecycle rules and publishes checked observations atomically. */
#if 0
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

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_financial_instrument_link_store_upsert(
    UmiFinancialInstrumentLinkStore *store,
    const UmiFinancialInstrumentLink *link)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (store != NULL && store->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

#endif

UmiStatus umi_financial_instrument_link_validate(const UmiFinancialInstrumentLink *row)
{
    static const UmiSnapshotTextField fields[] = {
        {"instrumentId", offsetof(UmiFinancialInstrumentLink, instrumentId), sizeof(((UmiFinancialInstrumentLink *)0)->instrumentId), 1},
        {"productId", offsetof(UmiFinancialInstrumentLink, productId), sizeof(((UmiFinancialInstrumentLink *)0)->productId), 1},
        {"currency", offsetof(UmiFinancialInstrumentLink, currency), sizeof(((UmiFinancialInstrumentLink *)0)->currency), 1},
        {"settlementConvention", offsetof(UmiFinancialInstrumentLink, settlementConvention), sizeof(((UmiFinancialInstrumentLink *)0)->settlementConvention), 0}
    };
    if (row == NULL || UmiSnapshotValidateTextFields(row, sizeof(*row), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        (row->active != 0 && row->active != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_instrument_link_store_validate(const UmiFinancialInstrumentLinkStore *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->count > UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (owner->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialInstrumentLink *row = &owner->links[i];
        if (umi_financial_instrument_link_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            const UmiFinancialInstrumentLink *previous = &owner->links[j];
            if (strcmp(row->instrumentId, previous->instrumentId) == 0) return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}
void umi_financial_instrument_link_store_init(UmiFinancialInstrumentLinkStore *owner)
{
    if (owner == NULL) return;
    memset(owner, 0, sizeof(*owner));
    owner->revision = 1U;
}
const UmiFinancialInstrumentLink *umi_financial_instrument_link_store_find(const UmiFinancialInstrumentLinkStore *owner,
    const char *instrumentId)
{
    char instrumentIdKey[64];
    if (FinancialControlQuery(instrumentId, instrumentIdKey, sizeof instrumentIdKey) != UMI_STATUS_OK) return NULL;
    if (umi_financial_instrument_link_store_validate(owner) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialInstrumentLink *row = &owner->links[i];
        if (strcmp(row->instrumentId, instrumentIdKey) == 0) return row;
    }
    return NULL;
}
UmiStatus umi_financial_instrument_link_store_upsert(UmiFinancialInstrumentLinkStore *owner, const UmiFinancialInstrumentLink *row)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_instrument_link_store_validate(owner);
    if (status == UMI_STATUS_OK) status = umi_financial_instrument_link_validate(row);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialInstrumentLink *current = &owner->links[i];
        if (strcmp(current->instrumentId, row->instrumentId) == 0) {
            if (row->version < current->version) return UMI_STATUS_INVALID_STATE;
            *current = *row;
            ++owner->revision;
            return UMI_STATUS_OK;
        }
    }
    if (owner->count == UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    owner->links[owner->count++] = *row;
    ++owner->revision;
    return UMI_STATUS_OK;
}
