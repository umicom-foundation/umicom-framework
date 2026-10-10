/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/health.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/health.h"

#include <string.h>

/*
 * SOURCE PRESERVATION: The original launch-plan-only health projection is
 * retained below. It correctly inspected the original five dispositions but
 * did not recognise the per-member runtime observations now made by the suite
 * runtime. The replacement preserves its public ABI and extends the recognised
 * cases without confusing a READY binary with a RUNNING process.
 */
#if 0
/*
 * Provide the integration health from plan operation used by this module and its client
 * applications.
 */
void umi_integration_health_from_plan(
    const UmiIntegrationLaunchPlan *plan,
    UmiIntegrationHealthSummary *out_summary)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_summary == NULL) {
        return;
    }
    (void)memset(out_summary, 0, sizeof(*out_summary));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) {
        return;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->count; ++index) {
        const UmiIntegrationLaunchItem *item = &plan->items[index];
        /* Apply this operation only while the related capability or state is available. */
        if (item->disposition == UMI_INTEGRATION_LAUNCH_READY ||
            item->disposition == UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING) {
            ++out_summary->available;
        }
        /* Apply this branch only when its contract condition is satisfied. */
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
#endif

/* Project the current per-member observation, if any, into the existing
 * summary. The counters labelled 'missing' continue to report PLAN absence:
 * an observed runtime failure is reflected in healthy/degraded, not rewritten
 * as a fictitious missing installation. */
void umi_integration_health_from_plan(
    const UmiIntegrationLaunchPlan *plan,
    UmiIntegrationHealthSummary *out_summary)
{
    size_t index;
    bool failed_required = false;
    bool failed_optional = false;

    if (out_summary == NULL)
        return;
    (void)memset(out_summary, 0, sizeof(*out_summary));
    if (plan == NULL || plan->count > UMI_INTEGRATION_MAX_MEMBERS)
        return;

    for (index = 0U; index < plan->count; ++index) {
        const UmiIntegrationLaunchItem *item = &plan->items[index];
        if (item->disposition == UMI_INTEGRATION_LAUNCH_READY ||
            item->disposition == UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING ||
            item->disposition == UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING)
            ++out_summary->available;
        if (item->disposition == UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING ||
            item->disposition == UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING)
            ++out_summary->running;
        if (item->disposition == UMI_INTEGRATION_LAUNCH_OBSERVED_FAILED) {
            if (item->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED)
                failed_required = true;
            else
                failed_optional = true;
        }
    }

    out_summary->missing_required = plan->missing_required;
    out_summary->missing_optional = plan->missing_optional;
    out_summary->healthy = plan->missing_required == 0U &&
                           plan->missing_optional == 0U &&
                           !failed_required && !failed_optional;
    out_summary->degraded = plan->missing_required == 0U &&
                            !failed_required &&
                            (plan->missing_optional > 0U || failed_optional);
}
