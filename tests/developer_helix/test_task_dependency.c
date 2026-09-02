/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_task_dependency.c
 *
 * PURPOSE:
 *   Regression coverage for represent a dependency edge between engineering tasks without owning the task graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/task_dependency.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixTaskDependency r; umi_helix_task_dependency_init(&r,"task_dependency"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_task_dependency_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.8; double s=umi_helix_task_dependency_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
