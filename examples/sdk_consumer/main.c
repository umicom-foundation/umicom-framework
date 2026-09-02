/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_consumer/main.c
 *
 * PURPOSE:
 *   Prove that an external C23 application can consume the installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/sdk/sdk.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkExportPlan plan;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_sdk_export_plan_init(&plan, ".", "0.9") != UMI_STATUS_OK) {
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_sdk_export_plan_add_target(
            &plan, "base", "Umicom::base", 1) != UMI_STATUS_OK) return 1;
    puts("Umicom Framework SDK consumer is ready.");
    return 0;
}
