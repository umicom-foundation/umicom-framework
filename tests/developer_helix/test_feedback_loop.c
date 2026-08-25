/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_feedback_loop.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate feedback events and determine whether to continue, repair, validate, rollback or stop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/feedback_loop.h"
int main(void){ UmiHelixFeedbackLoop l; umi_helix_feedback_loop_init(&l,2); UmiHelixFeedbackEvent e; umi_helix_feedback_event_init(&e,"build"); if(umi_helix_feedback_loop_observe(&l,&e,false)!=UMI_STATUS_OK){ return 1; } if(umi_helix_feedback_loop_decide(&l)!=UMI_HELIX_DECISION_ALLOW){ return 2; } if(umi_helix_feedback_loop_observe(&l,&e,false)!=UMI_STATUS_OK||umi_helix_feedback_loop_decide(&l)!=UMI_HELIX_DECISION_REVIEW){ return 3; } return 0; }
