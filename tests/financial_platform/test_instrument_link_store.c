/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_instrument_link_store.c
 *
 * PURPOSE:
 *   Verify monotonic instrument/product reference linkage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/financial_platform/instrument_link_store.h"

int main(void)
{
    UmiFinancialInstrumentLinkStore store;
    UmiFinancialInstrumentLink link = {0};
    umi_financial_instrument_link_store_init(&store);
    (void)strcpy(link.instrumentId, "GBPUSD");
    (void)strcpy(link.productId, "FX.SPOT");
    (void)strcpy(link.currency, "USD");
    link.version = 2U;
    link.active = 1;
    assert(umi_financial_instrument_link_store_upsert(&store, &link) == UMI_STATUS_OK);
    assert(umi_financial_instrument_link_store_find(&store, "GBPUSD") != NULL);
    link.version = 1U;
    assert(umi_financial_instrument_link_store_upsert(&store, &link) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
