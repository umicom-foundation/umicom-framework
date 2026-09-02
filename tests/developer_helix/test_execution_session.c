/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_execution_session.c
 *
 * PURPOSE:
 *   Regression coverage for track the running execution cursor, completed steps, failures and cancellation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/execution_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixExecutionSession s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_execution_session_start(&s,2)!=UMI_STATUS_OK){ return 1; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_execution_session_complete_step(&s,true)!=UMI_STATUS_OK||s.completed!=1U){ return 2; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_execution_session_complete_step(&s,true)!=UMI_STATUS_OK||s.state!=UMI_HELIX_SUCCEEDED){ return 3; } return 0; }
