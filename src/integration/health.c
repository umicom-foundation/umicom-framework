/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/health.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/health.h"

#include <string.h>

void umi_integration_health_from_plan(
    const UmiIntegrationLaunchPlan *plan,
    UmiIntegrationHealthSummary *out_summary)
{
    size_t index;
    if (out_summary == NULL) {
        return;
    }
    (void)memset(out_summary, 0, sizeof(*out_summary));
    if (plan == NULL) {
        return;
    }

    for (index = 0U; index < plan->count; ++index) {
        const UmiIntegrationLaunchItem *item = &plan->items[index];
        if (item->disposition == UMI_INTEGRATION_LAUNCH_READY ||
            item->disposition == UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING) {
            ++out_summary->available;
        }
        if (item->disposition ==
            UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING) {
            ++out_summary->running;
        }
    }

    out_summary->missing_required = plan->missing_required;
    out_summary->missing_optional = plan->missing_optional;
    out_summary->healthy =
        plan->missing_required == 0U && plan->missing_optional == 0U;
    out_summary->degraded =
        plan->missing_required == 0U && plan->missing_optional > 0U;
}
