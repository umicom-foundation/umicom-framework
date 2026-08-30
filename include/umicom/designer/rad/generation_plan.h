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
typedef struct UmiRadGenerationPlan {
    char application_id[UMI_RAD_ID_CAPACITY];
    char output_root[UMI_RAD_PATH_CAPACITY];
    size_t file_count;
    bool declarative_enabled;
    bool source_enabled;
} UmiRadGenerationPlan;
UmiStatus umi_rad_generation_plan_init(UmiRadGenerationPlan *item);
int umi_rad_generation_plan_is_valid(const UmiRadGenerationPlan *item);
#ifdef __cplusplus
}
#endif
#endif
