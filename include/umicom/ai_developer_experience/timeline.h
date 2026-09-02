/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/timeline.h
 *
 * PURPOSE:
 *   Retain a bounded unified AI developer timeline spanning agent state, tool
 *   calls, validation, approvals and patch review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TIMELINE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TIMELINE_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai developer timeline data shared with callers of this public contract.
 */
typedef struct UmiAiDeveloperTimeline UmiAiDeveloperTimeline;

/**
 * Initialise ai developer timeline from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_developer_timeline_create(
    UmiAiDeveloperTimeline **out_timeline);
/**
 * Release or reset state held by ai developer timeline so the same storage can be reused
 * safely.
 */
void umi_ai_developer_timeline_destroy(UmiAiDeveloperTimeline *timeline);

/**
 * Add ai developer timeline only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_developer_timeline_append(
    UmiAiDeveloperTimeline *timeline,
    const UmiAiDeveloperTimelineEvent *event);

/**
 * Find ai developer timeline while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_developer_timeline_at(
    const UmiAiDeveloperTimeline *timeline,
    size_t index,
    UmiAiDeveloperTimelineEvent *out_event);

/**
 * Return the number of records represented by ai developer timeline without changing their
 * state.
 */
size_t umi_ai_developer_timeline_count(
    const UmiAiDeveloperTimeline *timeline);

/**
 * Provide the ai developer timeline revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_developer_timeline_revision(
    const UmiAiDeveloperTimeline *timeline);

/**
 * Release or reset state held by ai developer timeline so the same storage can be reused
 * safely.
 */
void umi_ai_developer_timeline_clear(
    UmiAiDeveloperTimeline *timeline);

#ifdef __cplusplus
}
#endif
#endif
