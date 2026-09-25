/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/types.c
 *
 * PURPOSE:
 *   Initialise and validate broker-connectivity workbench evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/types.h"

#include <string.h>

void umi_broker_connectivity_input_init(UmiBrokerConnectivityInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->trusted = 1;
}

void umi_broker_connectivity_snapshot_init(UmiBrokerConnectivitySnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus umi_broker_connectivity_snapshot_validate(
    const UmiBrokerConnectivitySnapshot *snapshot)
{
    if (snapshot == NULL || snapshot->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (snapshot->score < 0.0 || snapshot->score > 100.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->ready && snapshot->blocked) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->approvalRequired && snapshot->ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
