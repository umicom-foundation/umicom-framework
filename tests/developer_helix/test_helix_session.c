/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_helix_session.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate goal, plan, budgets, checkpoints, validation and lifecycle state for one helix run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/helix_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixSession s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_init(&s,"session",3)!=UMI_STATUS_OK){ return 1; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_transition(&s,UMI_HELIX_PLANNED)!=UMI_STATUS_OK||umi_helix_session_transition(&s,UMI_HELIX_READY)!=UMI_STATUS_OK){ return 2; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_transition(&s,UMI_HELIX_RUNNING)!=UMI_STATUS_INVALID_STATE){ return 3; } s.approved=true; s.checkpoint_ready=true; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_transition(&s,UMI_HELIX_RUNNING)!=UMI_STATUS_OK||s.iteration!=1U){ return 4; } return 0; }
