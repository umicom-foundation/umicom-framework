/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/engineering_task.h
 *
 * PURPOSE:
 *   Describe an atomic engineering task with priority, lifecycle and acceptance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_ENGINEERING_TASK_H
#define UMICOM_DEVELOPER_HELIX_ENGINEERING_TASK_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix engineering task data shared with callers of this public contract.
 */
typedef struct UmiHelixEngineeringTask {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double progress;
} UmiHelixEngineeringTask;
/* Initialise a deterministic, empty engineering task record. */
void umi_helix_engineering_task_init(UmiHelixEngineeringTask *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_engineering_task_validate(const UmiHelixEngineeringTask *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_engineering_task_score(const UmiHelixEngineeringTask *record);
#ifdef __cplusplus
}
#endif
#endif
