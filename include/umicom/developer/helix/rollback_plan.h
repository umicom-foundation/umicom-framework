/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/rollback_plan.h
 *
 * PURPOSE:
 *   Describe reverse-order restoration steps and the conditions that make rollback mandatory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_ROLLBACK_PLAN_H
#define UMICOM_DEVELOPER_HELIX_ROLLBACK_PLAN_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRollbackPlan {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixRollbackPlan;
/* Initialise a deterministic, empty rollback plan record. */
void umi_helix_rollback_plan_init(UmiHelixRollbackPlan *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_rollback_plan_validate(const UmiHelixRollbackPlan *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_rollback_plan_score(const UmiHelixRollbackPlan *record);
#ifdef __cplusplus
}
#endif
#endif
