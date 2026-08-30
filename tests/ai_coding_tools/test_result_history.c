/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_result_history.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI coding tools result history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_tools/result_history.h"

int main(void)
{
    UmiAiCodingToolResultHistory *history = NULL;
    UmiAiCodingToolResult result = {0};
    UmiAiCodingToolResult copy;

    assert(umi_ai_coding_tool_result_history_create(&history) ==
           UMI_STATUS_OK);
    result.call_id = 1U;
    result.state = UMI_AI_CODING_TOOL_CALL_SUCCEEDED;
    (void)strcpy(result.tool_id, "workspace.exists");

    assert(umi_ai_coding_tool_result_history_record(
        history, &result) == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_result_history_count(history) == 1U);
    assert(umi_ai_coding_tool_result_history_at(
        history, 0U, &copy) == UMI_STATUS_OK);
    assert(copy.call_id == 1U);

    umi_ai_coding_tool_result_history_destroy(history);
    return 0;
}

