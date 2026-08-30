/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_engineering_task.c
 *
 * PURPOSE:
 *   Regression coverage for describe an atomic engineering task with priority, lifecycle and acceptance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/engineering_task.h"
int main(void){ UmiHelixEngineeringTask r; umi_helix_engineering_task_init(&r,"engineering_task"); if(umi_helix_engineering_task_validate(&r)!=UMI_STATUS_OK){ return 1; } r.progress=0.75; double s=umi_helix_engineering_task_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
