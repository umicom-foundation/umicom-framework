/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/observation.c
 *
 * PURPOSE:
 *   Coordinate admission before publishing related coding-agent observation records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/observation.h"

/* Stable local values let all stores admit the same records they will receive.
 * Each commit repeats its own admission, but no callback or external work can
 * change an admitted owner between these calls on the serialized owner thread. */
UmiStatus UmiAiCodingObservationPublish(UmiAiCodingEventQueue *events,
    UmiAiCodingHistory *history, UmiAiCodingAuditLog *audit,
    const UmiAiCodingEvent *event, const UmiAiCodingAgentSnapshot *snapshot,
    const UmiAiCodingAuditRecord *record)
{
    if (event == NULL || record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiCodingEvent stable_event = *event;
    UmiAiCodingAuditRecord stable_record = *record;
    UmiAiCodingAgentSnapshot stable_snapshot = {0};
    if (snapshot != NULL) stable_snapshot = *snapshot;
    UmiStatus status = UmiAiCodingEventQueueCheckPushBatch(events, &stable_event, 1U);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiCodingAuditLogCheckAppendBatch(audit, &stable_record, 1U);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot != NULL) {
        status = UmiAiCodingHistoryCheckRecordBatch(history, &stable_snapshot, 1U);
        if (status != UMI_STATUS_OK) return status;
    }
    /* All fallible admission has completed. These bounded memory operations
     * cannot refuse unchanged, valid owners and the stable admitted records. */
    status = UmiAiCodingAuditLogAppendBatch(audit, &stable_record, 1U);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot != NULL) {
        status = UmiAiCodingHistoryRecordBatch(history, &stable_snapshot, 1U);
        if (status != UMI_STATUS_OK) return status;
    }
    return UmiAiCodingEventQueuePushBatch(events, &stable_event, 1U);
}
