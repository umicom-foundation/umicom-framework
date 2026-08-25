/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/feedback_loop.c
 *
 * PURPOSE:
 *   Aggregate feedback events and determine whether to continue, repair, validate, rollback or stop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/feedback_loop.h"
#include <string.h>
/* Start with a non-zero bounded failure ceiling. */
void umi_helix_feedback_loop_init(UmiHelixFeedbackLoop *l,uint32_t m){ if(l==NULL){ return; } memset(l,0,sizeof(*l)); l->max_consecutive_failures=m==0U?1U:m; }
/* Aggregate feedback without executing repair or rollback directly. */
UmiStatus umi_helix_feedback_loop_observe(UmiHelixFeedbackLoop *l,const UmiHelixFeedbackEvent *e,bool passed){ if(l==NULL||e==NULL||umi_helix_feedback_event_validate(e)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } l->events++; if(passed) l->consecutive_failures=0U; else { l->failures++; l->consecutive_failures++; if(e->severity>=UMI_HELIX_CRITICAL) l->critical++; } return UMI_STATUS_OK; }
/* Escalate critical or repeated failure evidence conservatively. */
UmiHelixDecision umi_helix_feedback_loop_decide(const UmiHelixFeedbackLoop *l){ if(l==NULL){ return UMI_HELIX_DECISION_DENY; } if(l->critical>0U){ return UMI_HELIX_DECISION_ROLLBACK; } if(l->consecutive_failures>=l->max_consecutive_failures){ return UMI_HELIX_DECISION_REVIEW; } return UMI_HELIX_DECISION_ALLOW; }
