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

/* Validate complete caller-owned storage before traversal: bounded terminated
 * text, required identities, finite numeric observations, known states/boolean
 * flags and unique keys. Count must fit the array and revision must be nonzero.
 * These references do not replace canonical domain engines or authenticate
 * permissions. Keep inputs stable on one owner thread and outputs disjoint.
 * Borrowed find results last until the next mutation; queries inspect at most
 * 64 readable bytes and must terminate. Unused rows/text tails are ignored. */
UmiStatus umi_financial_instrument_link_validate(const UmiFinancialInstrumentLink *row);
UmiStatus umi_financial_instrument_link_store_validate(const UmiFinancialInstrumentLinkStore *owner);
/* settlementConvention remains optional. Equal versions may replace a row. */
void umi_financial_instrument_link_store_init(UmiFinancialInstrumentLinkStore *store);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
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
