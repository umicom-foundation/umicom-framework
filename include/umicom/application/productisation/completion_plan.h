/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/completion_plan.h
 *
 * PURPOSE:
 *   Convert productisation gaps into a deterministic Framework-first sequence,
 *   followed by Studio, Trader and remaining thin-application adoption.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_COMPLETION_PLAN_H
#define UMICOM_APPLICATION_PRODUCTISATION_COMPLETION_PLAN_H

#include "umicom/application/productisation/gap_analysis.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductisationCompletionStep {
    UmiProductisationStage stage;
    UmiProductisationSeverity severity;
    UmiProductisationOwner owner;
    UmiProductisationGapKind gap_kind;
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char asset_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char capability_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char action[UMI_PRODUCTISATION_TEXT_CAPACITY];
    size_t affected_application_count;
} UmiProductisationCompletionStep;

typedef struct UmiProductisationCompletionPlan {
    UmiProductisationCompletionStep steps[UMI_PRODUCTISATION_MAX_STEPS];
    size_t step_count;
    size_t framework_step_count;
    size_t studio_step_count;
    size_t trader_step_count;
    size_t remaining_application_step_count;
    size_t acceptance_step_count;
    int framework_ready_for_product_adoption;
    int studio_ready_for_completion;
    int trader_ready_for_completion;
    int capacity_exhausted;
} UmiProductisationCompletionPlan;

UmiStatus umi_productisation_completion_plan_build(
    const UmiProductisationGapReport *report,
    UmiProductisationCompletionPlan *out_plan);
const UmiProductisationCompletionStep *
umi_productisation_completion_plan_at(
    const UmiProductisationCompletionPlan *plan,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
