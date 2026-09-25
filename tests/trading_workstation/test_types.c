/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_workstation/test_types.c
 *
 * PURPOSE:
 *   Verify professional trading-workstation input and snapshot validation.
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

#include "umicom/trading_workstation/types.h"

int main(void)
{
    UmiTradingProfessionalInput input;
    UmiTradingProfessionalSnapshot snapshot;

    umi_trading_professional_input_init(&input);
    assert(input.trusted == 1);

    umi_trading_professional_snapshot_init(&snapshot);
    (void)strcpy(snapshot.id, "market.quote-midprice");
    snapshot.score = 50.0;
    snapshot.ratio = 0.0;
    snapshot.ready = 1;
    assert(umi_trading_professional_snapshot_validate(&snapshot) ==
           UMI_STATUS_OK);

    snapshot.score = 101.0;
    assert(umi_trading_professional_snapshot_validate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
