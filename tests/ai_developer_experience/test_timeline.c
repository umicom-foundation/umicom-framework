/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_timeline.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience timeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/timeline.h"

int main(void)
{
    UmiAiDeveloperTimeline *timeline = NULL;
    UmiAiDeveloperTimelineEvent event = {0};
    UmiAiDeveloperTimelineEvent copy;

    assert(umi_ai_developer_timeline_create(&timeline) == UMI_STATUS_OK);
    (void)strcpy(event.category, "tool");
    (void)strcpy(event.title, "Build");
    event.status = UMI_STATUS_OK;

    assert(umi_ai_developer_timeline_append(
        timeline, &event) == UMI_STATUS_OK);
    assert(umi_ai_developer_timeline_count(timeline) == 1U);
    assert(umi_ai_developer_timeline_at(
        timeline, 0U, &copy) == UMI_STATUS_OK);
    assert(copy.sequence != 0U);

    umi_ai_developer_timeline_destroy(timeline);
    return 0;
}

