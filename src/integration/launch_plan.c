/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/launch_plan.c
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

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/launch_plan.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_integration_launch_plan_build(
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry,
    UmiIntegrationLaunchPlan *out_plan)
{
    size_t index;

    if (suite == NULL || registry == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_plan, 0, sizeof(*out_plan));
    for (index = 0U; index < suite->member_count; ++index) {
        const UmiIntegrationSuiteMember *member = &suite->members[index];
        const UmiIntegrationRegistryEntry *entry =
            umi_integration_registry_find_const(
                registry, member->application_id);
        UmiIntegrationLaunchItem *item =
            &out_plan->items[out_plan->count];
        int written = snprintf(item->application_id,
                               sizeof(item->application_id),
                               "%s",
                               member->application_id);
        if (written < 0 ||
            (size_t)written >= sizeof(item->application_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        item->kind = member->kind;
        item->preferred_frontend = member->preferred_frontend;
        ++out_plan->count;

        if (entry == NULL) {
            item->disposition =
                member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED
                    ? UMI_INTEGRATION_LAUNCH_REQUIRED_MISSING
                    : UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING;
            if (member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
                ++out_plan->missing_required;
            } else {
                ++out_plan->missing_optional;
            }
        } else if (!entry->application.enabled) {
            item->disposition = UMI_INTEGRATION_LAUNCH_DISABLED;
            if (member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
                ++out_plan->missing_required;
            } else {
                ++out_plan->missing_optional;
            }
        } else if (entry->state == UMI_INTEGRATION_APP_RUNNING) {
            item->disposition = UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING;
            ++out_plan->ready_count;
        } else {
            item->disposition = UMI_INTEGRATION_LAUNCH_READY;
            ++out_plan->ready_count;
        }
    }

    return out_plan->missing_required == 0U
        ? UMI_STATUS_OK
        : UMI_STATUS_UNAVAILABLE;
}

bool umi_integration_launch_plan_can_start(
    const UmiIntegrationLaunchPlan *plan)
{
    return plan != NULL && plan->missing_required == 0U;
}
