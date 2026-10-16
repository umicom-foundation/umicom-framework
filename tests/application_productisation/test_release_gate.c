/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_release_gate.c
 *
 * PURPOSE:
 *   Verify catalogue gating and the explicit prohibition on live trading
 *   without accepted paper, OMS, risk and guarded-live evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

#include "umicom/application/productisation/runtime.h"
#include "umicom/application/application.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProductisationRuntime *runtime = NULL;
    assert(umi_productisation_runtime_create(&runtime) == UMI_STATUS_OK);
    assert(runtime != NULL);
    /* Release evidence must cover every registered product as the portfolio
     * grows. Preserve the earlier fixed count for catalogue-history review. */
#if 0
    assert(runtime->release_gate.application_count == 25U);
#endif
    assert(umi_application_portfolio_find("org.umicom.healthcare") != NULL);
    assert(runtime->release_gate.application_count == umi_application_portfolio_count());
    assert(runtime->release_gate.experience_catalogue_valid);
    assert(!runtime->release_gate.trader_live_execution_allowed);
    assert(runtime->revision == 1U);
    umi_productisation_runtime_destroy(runtime);
    return 0;
}
