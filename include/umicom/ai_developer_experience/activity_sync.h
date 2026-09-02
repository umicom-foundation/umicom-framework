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

/**
 * Represent the ai developer activity cursor data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperActivityCursor {
    size_t tool_result_count;
    uint64_t validation_revision;
    size_t validation_result_count;
} UmiAiDeveloperActivityCursor;

/**
 * Initialise ai developer activity cursor from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_developer_activity_cursor_init(
    UmiAiDeveloperActivityCursor *cursor);

/**
 * Provide the ai developer activity sync tools operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_activity_sync_tools(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingToolResultHistory *history,
    UmiAiDeveloperTimeline *timeline);

/**
 * Provide the ai developer activity sync validation operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_activity_sync_validation(
    UmiAiDeveloperActivityCursor *cursor,
    const char *task_id,
    const UmiAiCodingValidationReport *report,
    UmiAiDeveloperTimeline *timeline);

#ifdef __cplusplus
}
#endif
#endif
