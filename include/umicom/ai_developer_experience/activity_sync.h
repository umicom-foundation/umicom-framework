/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/activity_sync.h
 *
 * PURPOSE:
 *   Incrementally synchronize controlled tool results and coding validation into
 *   the unified AI developer timeline without duplicating old events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_ACTIVITY_SYNC_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_ACTIVITY_SYNC_H

#include "umicom/ai_developer_experience/timeline_projection.h"
#include "umicom/ai_coding_tools/result_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperActivityCursor {
    size_t tool_result_count;
    uint64_t validation_revision;
    size_t validation_result_count;
} UmiAiDeveloperActivityCursor;

void umi_ai_developer_activity_cursor_init(
    UmiAiDeveloperActivityCursor *cursor);

UmiStatus umi_ai_developer_activity_sync_tools(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingToolResultHistory *history,
    UmiAiDeveloperTimeline *timeline);

UmiStatus umi_ai_developer_activity_sync_validation(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingValidationReport *report,
    UmiAiDeveloperTimeline *timeline);

#ifdef __cplusplus
}
#endif
#endif
