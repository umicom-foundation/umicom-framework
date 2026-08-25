/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/task_dependency.h
 *
 * PURPOSE:
 *   Represent a dependency edge between engineering tasks without owning the task graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_TASK_DEPENDENCY_H
#define UMICOM_DEVELOPER_HELIX_TASK_DEPENDENCY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixTaskDependency {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixTaskDependency;
/* Initialise a deterministic, empty task dependency record. */
void umi_helix_task_dependency_init(UmiHelixTaskDependency *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_task_dependency_validate(const UmiHelixTaskDependency *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_task_dependency_score(const UmiHelixTaskDependency *record);
#ifdef __cplusplus
}
#endif
#endif
