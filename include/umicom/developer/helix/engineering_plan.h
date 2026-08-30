/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/engineering_plan.h
 *
 * PURPOSE:
 *   Aggregate engineering tasks into a revisioned plan with explicit readiness and approval state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_ENGINEERING_PLAN_H
#define UMICOM_DEVELOPER_HELIX_ENGINEERING_PLAN_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixEngineeringPlan {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixEngineeringPlan;
/* Initialise a deterministic, empty engineering plan record. */
void umi_helix_engineering_plan_init(UmiHelixEngineeringPlan *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_engineering_plan_validate(const UmiHelixEngineeringPlan *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_engineering_plan_score(const UmiHelixEngineeringPlan *record);
#ifdef __cplusplus
}
#endif
#endif
