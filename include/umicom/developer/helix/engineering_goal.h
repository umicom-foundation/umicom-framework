/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/engineering_goal.h
 *
 * PURPOSE:
 *   Represent a measurable engineering outcome and progress evidence for a governed Helix session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_ENGINEERING_GOAL_H
#define UMICOM_DEVELOPER_HELIX_ENGINEERING_GOAL_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixEngineeringGoal {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixEngineeringGoal;
/* Initialise a deterministic, empty engineering goal record. */
void umi_helix_engineering_goal_init(UmiHelixEngineeringGoal *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_engineering_goal_validate(const UmiHelixEngineeringGoal *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_engineering_goal_score(const UmiHelixEngineeringGoal *record);
#ifdef __cplusplus
}
#endif
#endif
