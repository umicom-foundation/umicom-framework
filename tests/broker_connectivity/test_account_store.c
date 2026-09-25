/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_account_store.c
 *
 * PURPOSE:
 *   Verify broker account upsert and monotonic provider sequence handling.
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
#include "umicom/broker_connectivity/account_store.h"

int main(void)
{
    UmiBrokerAccountStore store;
    UmiBrokerAccountSnapshot snapshot = {0};

    umi_broker_account_store_init(&store);
    (void)strcpy(snapshot.accountId, "DU123");
    (void)strcpy(snapshot.baseCurrency, "USD");
    snapshot.netLiquidation = 100000.0;
    snapshot.providerSequence = 2U;
    assert(umi_broker_account_store_upsert(&store, &snapshot) == UMI_STATUS_OK);
    assert(umi_broker_account_store_find(&store, "DU123") != NULL);

    snapshot.providerSequence = 1U;
    assert(umi_broker_account_store_upsert(&store, &snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
