/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_types.c
 *
 * PURPOSE:
 *   Verify broker-connectivity input defaults and snapshot validation.
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
#include "umicom/broker_connectivity/types.h"

int main(void)
{
    UmiBrokerConnectivityInput input;
    UmiBrokerConnectivitySnapshot snapshot;
    umi_broker_connectivity_input_init(&input);
    assert(input.trusted == 1);

    umi_broker_connectivity_snapshot_init(&snapshot);
    (void)strcpy(snapshot.id, "provider.health");
    snapshot.score = 90.0;
    snapshot.ready = 1;
    assert(umi_broker_connectivity_snapshot_validate(&snapshot) == UMI_STATUS_OK);
    snapshot.score = 101.0;
    assert(umi_broker_connectivity_snapshot_validate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
