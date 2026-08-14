/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_consumer/main.c
 *
 * PURPOSE:
 *   Prove that an external C23 application can consume the installed SDK.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/sdk/sdk.h"

int main(void)
{
    UmiSdkExportPlanV2 plan;
    if (umi_sdk_export_plan_v2_init(&plan, ".", "0.9") != UMI_STATUS_OK) {
        return 1;
    }
    if (umi_sdk_export_plan_v2_add_target(
            &plan, "base", "Umicom::base", 1) != UMI_STATUS_OK) return 1;
    puts("Umicom Framework SDK consumer is ready.");
    return 0;
}
