/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/patch_plan.h
 *
 * PURPOSE:
 *   Order selected patch candidates and enforce bounded, reviewable patch application policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_PATCH_PLAN_H
#define UMICOM_DEVELOPER_HELIX_PATCH_PLAN_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixPatchPlan {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixPatchPlan;
/* Initialise a deterministic, empty patch plan record. */
void umi_helix_patch_plan_init(UmiHelixPatchPlan *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_patch_plan_validate(const UmiHelixPatchPlan *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_patch_plan_score(const UmiHelixPatchPlan *record);
#ifdef __cplusplus
}
#endif
#endif
