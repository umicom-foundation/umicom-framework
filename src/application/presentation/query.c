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

/*
 * Return the number of records represented by application presentation plan region without
 * changing their state.
 */
size_t umi_application_presentation_plan_region_count(
    const UmiApplicationPresentationPlan *plan,
    UmiApplicationComponentRegion region)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->placement_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (plan->placements[index].slot->region == region) count += 1U;
    }
    return count;
}

/*
 * Find application presentation plan region while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationPresentationPanelPlacement *
umi_application_presentation_plan_region_at(
    const UmiApplicationPresentationPlan *plan,
    UmiApplicationComponentRegion region,
    size_t region_index)
{
    size_t index;
    size_t match_index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->placement_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (plan->placements[index].slot->region != region) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (match_index == region_index) return &plan->placements[index];
        match_index += 1U;
    }
    return NULL;
}
