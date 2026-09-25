/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/instrument_link_store.h
 *
 * PURPOSE:
 *   Link canonical instrument identifiers to products, currencies and
 *   settlement conventions without duplicating instrument master data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_INSTRUMENT_LINK_STORE_H
#define UMICOM_FINANCIAL_PLATFORM_INSTRUMENT_LINK_STORE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY 512U

typedef struct UmiFinancialInstrumentLink {
    char instrumentId[64];
    char productId[64];
    char currency[8];
    char settlementConvention[32];
    uint64_t version;
    int active;
} UmiFinancialInstrumentLink;

typedef struct UmiFinancialInstrumentLinkStore {
    UmiFinancialInstrumentLink links[UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialInstrumentLinkStore;

void umi_financial_instrument_link_store_init(UmiFinancialInstrumentLinkStore *store);
UmiStatus umi_financial_instrument_link_store_upsert(
    UmiFinancialInstrumentLinkStore *store,
    const UmiFinancialInstrumentLink *link);
const UmiFinancialInstrumentLink *umi_financial_instrument_link_store_find(
    const UmiFinancialInstrumentLinkStore *store,
    const char *instrumentId);

#ifdef __cplusplus
}
#endif
#endif
