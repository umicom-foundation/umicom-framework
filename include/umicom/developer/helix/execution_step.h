/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/execution_step.h
 *
 * PURPOSE:
 *   Represent one planned executable step and its precondition/postcondition evidence state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_EXECUTION_STEP_H
#define UMICOM_DEVELOPER_HELIX_EXECUTION_STEP_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixExecutionStep {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixExecutionStep;
/* Initialise a deterministic, empty execution step record. */
void umi_helix_execution_step_init(UmiHelixExecutionStep *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_execution_step_validate(const UmiHelixExecutionStep *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_execution_step_score(const UmiHelixExecutionStep *record);
#ifdef __cplusplus
}
#endif
#endif
