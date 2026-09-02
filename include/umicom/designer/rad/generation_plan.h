/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/generation_plan.h
 *
 * PURPOSE:
 *   Describe generated declarative/source artifacts before filesystem writes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_GENERATION_PLAN_H
#define UMICOM_DESIGNER_RAD_GENERATION_PLAN_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad generation plan data shared with callers of this public contract.
 */
typedef struct UmiRadGenerationPlan {
    char application_id[UMI_RAD_ID_CAPACITY];
    char output_root[UMI_RAD_PATH_CAPACITY];
    size_t file_count;
    bool declarative_enabled;
    bool source_enabled;
} UmiRadGenerationPlan;
/**
 * Initialise rad generation plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_generation_plan_init(UmiRadGenerationPlan *item);
/**
 * Check that rad generation plan satisfies its contract before another service relies on
 * it.
 */
int umi_rad_generation_plan_is_valid(const UmiRadGenerationPlan *item);
#ifdef __cplusplus
}
#endif
#endif
