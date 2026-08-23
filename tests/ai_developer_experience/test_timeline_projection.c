/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_timeline_projection.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience timeline projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/timeline_projection.h"

int main(void)
{
    UmiAiCodingToolResult result = {0};
    UmiAiDeveloperTimelineEvent event;

    result.call_id = 3U;
    result.sequence = 9U;
    result.state = UMI_AI_CODING_TOOL_CALL_FAILED;
    result.status = UMI_STATUS_UNAVAILABLE;
    (void)strcpy(result.tool_id, "developer.build");
    (void)strcpy(result.output, "compiler error");

    assert(umi_ai_developer_timeline_from_tool_result(
        "task.1", &result, &event) == UMI_STATUS_OK);
    assert(strcmp(event.category, "tool") == 0);
    assert(event.important);
    assert(strstr(event.detail, "compiler error") != NULL);
    return 0;
}

