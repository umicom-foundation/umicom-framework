/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/evolution_goal.h
 *
 * PURPOSE:
 *   Describe a bounded software-evolution objective with measurable fitness and compatibility constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_EVOLUTION_GOAL_H
#define UMICOM_DEVELOPER_HELIX_EVOLUTION_GOAL_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixEvolutionGoal {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixEvolutionGoal;
/* Initialise a deterministic, empty evolution goal record. */
void umi_helix_evolution_goal_init(UmiHelixEvolutionGoal *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_evolution_goal_validate(const UmiHelixEvolutionGoal *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_evolution_goal_score(const UmiHelixEvolutionGoal *record);
#ifdef __cplusplus
}
#endif
#endif
