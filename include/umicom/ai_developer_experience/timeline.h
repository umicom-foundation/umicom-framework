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

typedef struct UmiAiDeveloperTimeline UmiAiDeveloperTimeline;

UmiStatus umi_ai_developer_timeline_create(
    UmiAiDeveloperTimeline **out_timeline);
void umi_ai_developer_timeline_destroy(UmiAiDeveloperTimeline *timeline);

UmiStatus umi_ai_developer_timeline_append(
    UmiAiDeveloperTimeline *timeline,
    const UmiAiDeveloperTimelineEvent *event);

UmiStatus umi_ai_developer_timeline_at(
    const UmiAiDeveloperTimeline *timeline,
    size_t index,
    UmiAiDeveloperTimelineEvent *out_event);

size_t umi_ai_developer_timeline_count(
    const UmiAiDeveloperTimeline *timeline);

uint64_t umi_ai_developer_timeline_revision(
    const UmiAiDeveloperTimeline *timeline);

void umi_ai_developer_timeline_clear(
    UmiAiDeveloperTimeline *timeline);

#ifdef __cplusplus
}
#endif
#endif
