/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_history_view.c
 *
 * PURPOSE:
 *   Toolkit-neutral view coverage for AI Developer Experience history view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/ai_developer_experience/views/history.h"
#include "umicom/ui/view_model.h"

int main(void)
{
    UmiAiDeveloperTimeline *timeline = NULL;
    UmiAiDeveloperTimelineEvent event = {0};
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(umi_ai_developer_timeline_create(&timeline) == UMI_STATUS_OK);
    (void)strcpy(event.category, "tool");
    (void)strcpy(event.title, "Build");
    (void)strcpy(event.detail, "Build completed");
    assert(umi_ai_developer_timeline_append(timeline, &event) == UMI_STATUS_OK);

    assert(umi_ai_developer_history_view_create(
        "test.history", timeline, 10U, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-history.row-count", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 1);

    umi_ui_view_model_destroy(view);
    umi_ai_developer_timeline_destroy(timeline);
    return 0;
}

