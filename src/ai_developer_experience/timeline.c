/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/timeline.c
 *
 * PURPOSE:
 *   Implement oldest-first bounded timeline retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/timeline.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperTimeline {
    UmiAiDeveloperTimelineEvent items[UMI_AI_DEVELOPER_TIMELINE_CAPACITY];
    size_t count;
    uint64_t revision;
};

UmiStatus umi_ai_developer_timeline_create(
    UmiAiDeveloperTimeline **out_timeline)
{
    UmiAiDeveloperTimeline *timeline;

    if (out_timeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_timeline = NULL;

    timeline = (UmiAiDeveloperTimeline *)calloc(1U, sizeof(*timeline));
    if (timeline == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    timeline->revision = 1U;
    *out_timeline = timeline;
    return UMI_STATUS_OK;
}

void umi_ai_developer_timeline_destroy(UmiAiDeveloperTimeline *timeline)
{
    free(timeline);
}

UmiStatus umi_ai_developer_timeline_append(
    UmiAiDeveloperTimeline *timeline,
    const UmiAiDeveloperTimelineEvent *event)
{
    if (timeline == NULL || event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (timeline->count == UMI_AI_DEVELOPER_TIMELINE_CAPACITY) {
        (void)memmove(
            &timeline->items[0],
            &timeline->items[1],
            (timeline->count - 1U) * sizeof(timeline->items[0]));
        timeline->count -= 1U;
    }

    timeline->items[timeline->count] = *event;

    if (timeline->items[timeline->count].sequence == 0U) {
        timeline->items[timeline->count].sequence =
            timeline->revision + 1U;
    }

    timeline->count += 1U;
    timeline->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_timeline_at(
    const UmiAiDeveloperTimeline *timeline,
    size_t index,
    UmiAiDeveloperTimelineEvent *out_event)
{
    if (timeline == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= timeline->count) return UMI_STATUS_NOT_FOUND;

    *out_event = timeline->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ai_developer_timeline_count(
    const UmiAiDeveloperTimeline *timeline)
{
    return timeline != NULL ? timeline->count : 0U;
}

uint64_t umi_ai_developer_timeline_revision(
    const UmiAiDeveloperTimeline *timeline)
{
    return timeline != NULL ? timeline->revision : 0U;
}

void umi_ai_developer_timeline_clear(
    UmiAiDeveloperTimeline *timeline)
{
    if (timeline == NULL) return;

    (void)memset(timeline->items, 0, sizeof(timeline->items));
    timeline->count = 0U;
    timeline->revision += 1U;
}
