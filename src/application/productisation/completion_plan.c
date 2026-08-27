/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/completion_plan.c
 *
 * PURPOSE:
 *   Order all portfolio gaps so shared Framework completion precedes Studio,
 *   Trader and the remaining product compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/completion_plan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiProductisationStage stage_for_gap(
    const UmiProductisationGap *gap)
{
    if (gap->owner == UMI_PRODUCTISATION_OWNER_FRAMEWORK)
        return UMI_PRODUCTISATION_STAGE_FRAMEWORK;
    if (gap->kind == UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE)
        return UMI_PRODUCTISATION_STAGE_ACCEPTANCE;
    if (strcmp(gap->application_id, "org.umicom.studio") == 0)
        return UMI_PRODUCTISATION_STAGE_STUDIO;
    if (strcmp(gap->application_id, "org.umicom.trader") == 0)
        return UMI_PRODUCTISATION_STAGE_TRADER;
    return UMI_PRODUCTISATION_STAGE_REMAINING_APPLICATIONS;
}

static int compare_steps(const void *left, const void *right)
{
    const UmiProductisationCompletionStep *a = left;
    const UmiProductisationCompletionStep *b = right;
    if (a->stage != b->stage) return (int)a->stage - (int)b->stage;
    if (a->severity != b->severity)
        return (int)b->severity - (int)a->severity;
    if (a->affected_application_count != b->affected_application_count)
        return a->affected_application_count < b->affected_application_count
            ? 1 : -1;
    {
        const int application_order =
            strcmp(a->application_id, b->application_id);
        if (application_order != 0) return application_order;
    }
    return strcmp(a->asset_id, b->asset_id);
}

static void count_stage(UmiProductisationCompletionPlan *plan,
                        UmiProductisationStage stage)
{
    if (stage == UMI_PRODUCTISATION_STAGE_FRAMEWORK)
        plan->framework_step_count += 1U;
    else if (stage == UMI_PRODUCTISATION_STAGE_STUDIO)
        plan->studio_step_count += 1U;
    else if (stage == UMI_PRODUCTISATION_STAGE_TRADER)
        plan->trader_step_count += 1U;
    else if (stage == UMI_PRODUCTISATION_STAGE_ACCEPTANCE)
        plan->acceptance_step_count += 1U;
    else plan->remaining_application_step_count += 1U;
}

UmiStatus umi_productisation_completion_plan_build(
    const UmiProductisationGapReport *report,
    UmiProductisationCompletionPlan *out_plan)
{
    size_t index;
    int framework_blocked = 0;
    int studio_blocked = 0;
    int trader_blocked = 0;
    if (report == NULL || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_plan, 0, sizeof(*out_plan));
    for (index = 0U; index < report->gap_count; ++index) {
        const UmiProductisationGap *gap = &report->gaps[index];
        UmiProductisationCompletionStep *step;
        if (out_plan->step_count >= UMI_PRODUCTISATION_MAX_STEPS) {
            out_plan->capacity_exhausted = 1;
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        step = &out_plan->steps[out_plan->step_count++];
        (void)memset(step, 0, sizeof(*step));
        step->stage = stage_for_gap(gap);
        step->severity = gap->severity;
        step->owner = gap->owner;
        step->gap_kind = gap->kind;
        step->affected_application_count =
            gap->affected_application_count;
        (void)snprintf(step->application_id,
                       sizeof(step->application_id), "%s",
                       gap->application_id);
        (void)snprintf(step->asset_id, sizeof(step->asset_id), "%s",
                       gap->asset_id);
        (void)snprintf(step->capability_id,
                       sizeof(step->capability_id), "%s",
                       gap->capability_id);
        (void)snprintf(step->action, sizeof(step->action), "%s",
                       gap->summary);
        count_stage(out_plan, step->stage);
        if (step->severity >= UMI_PRODUCTISATION_SEVERITY_HIGH) {
            if (step->stage == UMI_PRODUCTISATION_STAGE_FRAMEWORK)
                framework_blocked = 1;
            else if (strcmp(step->application_id,
                            "org.umicom.studio") == 0)
                studio_blocked = 1;
            else if (strcmp(step->application_id,
                            "org.umicom.trader") == 0)
                trader_blocked = 1;
        }
    }
    qsort(out_plan->steps, out_plan->step_count,
          sizeof(out_plan->steps[0]), compare_steps);
    out_plan->framework_ready_for_product_adoption = !framework_blocked;
    out_plan->studio_ready_for_completion = !framework_blocked &&
                                             !studio_blocked;
    out_plan->trader_ready_for_completion = !framework_blocked &&
                                             !trader_blocked;
    return UMI_STATUS_OK;
}

const UmiProductisationCompletionStep *
umi_productisation_completion_plan_at(
    const UmiProductisationCompletionPlan *plan,
    size_t index)
{
    return plan != NULL && index < plan->step_count
        ? &plan->steps[index] : NULL;
}
