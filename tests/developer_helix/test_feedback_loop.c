/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_feedback_loop.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate feedback events and determine whether to continue, repair, validate, rollback or stop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/feedback_loop.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixFeedbackLoop l; umi_helix_feedback_loop_init(&l,2); UmiHelixFeedbackEvent e; umi_helix_feedback_event_init(&e,"build"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_feedback_loop_observe(&l,&e,false)!=UMI_STATUS_OK){ return 1; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_feedback_loop_decide(&l)!=UMI_HELIX_DECISION_ALLOW){ return 2; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_feedback_loop_observe(&l,&e,false)!=UMI_STATUS_OK||umi_helix_feedback_loop_decide(&l)!=UMI_HELIX_DECISION_REVIEW){ return 3; } return 0; }
