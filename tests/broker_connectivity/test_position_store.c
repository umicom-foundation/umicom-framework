/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_position_store.c
 *
 * PURPOSE:
 *   Verify position identity and monotonic update handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../value_archive/revision_cases.h"
#include <assert.h>
#include <string.h>
#include "umicom/broker_connectivity/position_store.h"

int main(void)
{
    UmiBrokerPositionStore store;
    UmiBrokerPositionSnapshot position = {0};

    umi_broker_position_store_init(&store);
    (void)strcpy(position.accountId, "DU123");
    (void)strcpy(position.instrumentId, "CME.ES");
    (void)strcpy(position.symbol, "ES");
    position.quantity = 2.0;
    position.providerSequence = 5U;
    assert(umi_broker_position_store_upsert(&store, &position) == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiBrokerPositionStore, &store,
        (umi_broker_position_store_upsert(&store, &position)));

    assert(umi_broker_position_store_find(&store, "DU123", "CME.ES") != NULL);
    position.providerSequence = 4U;
    assert(umi_broker_position_store_upsert(&store, &position) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
