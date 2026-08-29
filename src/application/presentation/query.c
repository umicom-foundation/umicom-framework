/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/query.c
 *
 * PURPOSE:
 *   Implement safe region-based access to ordered application presentation
 *   plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/query.h"

size_t umi_application_presentation_plan_region_count(
    const UmiApplicationPresentationPlan *plan,
    UmiApplicationComponentRegion region)
{
    size_t index;
    size_t count = 0U;
    if (plan == NULL) return 0U;
    for (index = 0U; index < plan->placement_count; ++index) {
        if (plan->placements[index].slot->region == region) count += 1U;
    }
    return count;
}

const UmiApplicationPresentationPanelPlacement *
umi_application_presentation_plan_region_at(
    const UmiApplicationPresentationPlan *plan,
    UmiApplicationComponentRegion region,
    size_t region_index)
{
    size_t index;
    size_t match_index = 0U;
    if (plan == NULL) return NULL;
    for (index = 0U; index < plan->placement_count; ++index) {
        if (plan->placements[index].slot->region != region) continue;
        if (match_index == region_index) return &plan->placements[index];
        match_index += 1U;
    }
    return NULL;
}
