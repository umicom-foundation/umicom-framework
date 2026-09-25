/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_types.c
 *
 * PURPOSE:
 *   Verify financial-platform input defaults and snapshot validation.
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
#include "umicom/financial_platform/types.h"

int main(void)
{
    UmiFinancialPlatformInput input;
    UmiFinancialPlatformSnapshot snapshot;

    umi_financial_platform_input_init(&input);
    assert(input.trusted == 1);

    umi_financial_platform_snapshot_init(&snapshot);
    (void)strcpy(snapshot.id, "audit.readiness");
    snapshot.score = 90.0;
    snapshot.ready = 1;
    assert(umi_financial_platform_snapshot_validate(&snapshot) ==
           UMI_STATUS_OK);

    snapshot.score = 101.0;
    assert(umi_financial_platform_snapshot_validate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
