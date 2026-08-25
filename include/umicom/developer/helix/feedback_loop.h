/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/feedback_loop.h
 *
 * PURPOSE:
 *   Aggregate feedback events and determine whether to continue, repair, validate, rollback or stop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_FEEDBACK_LOOP_H
#define UMICOM_DEVELOPER_HELIX_FEEDBACK_LOOP_H
#include "umicom/developer/helix/feedback_event.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixFeedbackLoop { size_t events; size_t failures; size_t critical; size_t consecutive_failures; uint32_t max_consecutive_failures; } UmiHelixFeedbackLoop;
/* Initialise feedback-loop escalation limits. */
void umi_helix_feedback_loop_init(UmiHelixFeedbackLoop *loop,uint32_t max_consecutive_failures);
/* Consume one feedback event and update failure/escalation evidence. */
UmiStatus umi_helix_feedback_loop_observe(UmiHelixFeedbackLoop *loop,const UmiHelixFeedbackEvent *event,bool passed);
/* Decide whether work may continue, requires review or must stop. */
UmiHelixDecision umi_helix_feedback_loop_decide(const UmiHelixFeedbackLoop *loop);
#ifdef __cplusplus
}
#endif
#endif
