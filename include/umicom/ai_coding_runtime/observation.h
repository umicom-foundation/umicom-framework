/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/observation.h
 *
 * PURPOSE:
 *   Publish related coding-agent event, history and audit records after shared admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_CODING_RUNTIME_OBSERVATION_H
#define UMICOM_AI_CODING_RUNTIME_OBSERVATION_H
#include "umicom/ai_coding_runtime/event_queue.h"
#include "umicom/ai_coding_runtime/history.h"
#include "umicom/ai_coding_runtime/audit.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Admit every participating store before changing any of them. event and audit
 * are required; snapshot is optional. A non-NULL snapshot requires history.
 * If snapshot is NULL, history is ignored and no history record is appended.
 * Inputs are copied before publication; they must describe valid readable values.
 *
 * Call only on the serialized owner thread with no concurrent changes to the
 * stores or inputs. There are no callbacks, allocation or external operations
 * between admission and publication. This is in-memory composition, not a lock,
 * disk transaction or guarantee of crash durability. A normal admission refusal
 * leaves all stores unchanged. Event and audit sequences retain their respective
 * existing producer-owned and log-owned meanings. */
UmiStatus UmiAiCodingObservationPublish(UmiAiCodingEventQueue *events,
    UmiAiCodingHistory *history, UmiAiCodingAuditLog *audit,
    const UmiAiCodingEvent *event, const UmiAiCodingAgentSnapshot *snapshot,
    const UmiAiCodingAuditRecord *record);
#ifdef __cplusplus
}
#endif
#endif
