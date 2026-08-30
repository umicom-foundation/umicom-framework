/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_execution_step.c
 *
 * PURPOSE:
 *   Regression coverage for represent one planned executable step and its precondition/postcondition evidence state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/execution_step.h"
int main(void){ UmiHelixExecutionStep r; umi_helix_execution_step_init(&r,"execution_step"); if(umi_helix_execution_step_validate(&r)!=UMI_STATUS_OK){ return 1; } r.progress=0.75; double s=umi_helix_execution_step_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
