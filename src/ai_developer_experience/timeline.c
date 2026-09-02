/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/timeline.c
 *
 * PURPOSE:
 *   Implement oldest-first bounded timeline retention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/timeline.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperTimeline {
    UmiAiDeveloperTimelineEvent items[UMI_AI_DEVELOPER_TIMELINE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/*
 * Initialise ai developer timeline from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_developer_timeline_create(
    UmiAiDeveloperTimeline **out_timeline)
{
    UmiAiDeveloperTimeline *timeline;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_timeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_timeline = NULL;

    timeline = (UmiAiDeveloperTimeline *)calloc(1U, sizeof(*timeline));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    timeline->revision = 1U;
    *out_timeline = timeline;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai developer timeline so the same storage can be reused
 * safely.
 */
void umi_ai_developer_timeline_destroy(UmiAiDeveloperTimeline *timeline)
{
    free(timeline);
}

/*
 * Add ai developer timeline only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_developer_timeline_append(
    UmiAiDeveloperTimeline *timeline,
    const UmiAiDeveloperTimelineEvent *event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL || event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (timeline->count == UMI_AI_DEVELOPER_TIMELINE_CAPACITY) {
        (void)memmove(
            &timeline->items[0],
            &timeline->items[1],
            (timeline->count - 1U) * sizeof(timeline->items[0]));
        timeline->count -= 1U;
    }

    timeline->items[timeline->count] = *event;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (timeline->items[timeline->count].sequence == 0U) {
        timeline->items[timeline->count].sequence =
            timeline->revision + 1U;
    }

    timeline->count += 1U;
    timeline->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find ai developer timeline while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_developer_timeline_at(
    const UmiAiDeveloperTimeline *timeline,
    size_t index,
    UmiAiDeveloperTimelineEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= timeline->count) return UMI_STATUS_NOT_FOUND;

    *out_event = timeline->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai developer timeline without changing their
 * state.
 */
size_t umi_ai_developer_timeline_count(
    const UmiAiDeveloperTimeline *timeline)
{
    return timeline != NULL ? timeline->count : 0U;
}

/*
 * Provide the ai developer timeline revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_developer_timeline_revision(
    const UmiAiDeveloperTimeline *timeline)
{
    return timeline != NULL ? timeline->revision : 0U;
}

/*
 * Release or reset state held by ai developer timeline so the same storage can be reused
 * safely.
 */
void umi_ai_developer_timeline_clear(
    UmiAiDeveloperTimeline *timeline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL) return;

    (void)memset(timeline->items, 0, sizeof(timeline->items));
    timeline->count = 0U;
    timeline->revision += 1U;
}
