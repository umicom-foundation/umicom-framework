/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/types.c
 *
 * PURPOSE:
 *   Initialise and validate shared professional trading-workstation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/types.h"

#include <string.h>

void umi_trading_professional_input_init(
    UmiTradingProfessionalInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->trusted = 1;
}

void umi_trading_professional_snapshot_init(
    UmiTradingProfessionalSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus umi_trading_professional_snapshot_validate(
    const UmiTradingProfessionalSnapshot *snapshot)
{
    if (snapshot == NULL || snapshot->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (snapshot->score < 0.0 || snapshot->score > 100.0 ||
        snapshot->ratio < -1000.0 || snapshot->ratio > 1000.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->ready && snapshot->blocked) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
