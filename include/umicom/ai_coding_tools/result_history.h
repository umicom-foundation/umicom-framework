/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/result_history.h
 *
 * PURPOSE:
 *   Retain bounded tool-call results for agent reasoning, workbench history and
 *   audit without making result lifetime depend on a caller stack frame.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_RESULT_HISTORY_H
#define UMICOM_AI_CODING_TOOLS_RESULT_HISTORY_H
#include "umicom/ai_coding_tools/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingToolResultHistory UmiAiCodingToolResultHistory;

UmiStatus umi_ai_coding_tool_result_history_create(
    UmiAiCodingToolResultHistory **out_history);
void umi_ai_coding_tool_result_history_destroy(
    UmiAiCodingToolResultHistory *history);
UmiStatus umi_ai_coding_tool_result_history_record(
    UmiAiCodingToolResultHistory *history,
    const UmiAiCodingToolResult *result);
UmiStatus umi_ai_coding_tool_result_history_at(
    const UmiAiCodingToolResultHistory *history,
    size_t index,
    UmiAiCodingToolResult *out_result);
size_t umi_ai_coding_tool_result_history_count(
    const UmiAiCodingToolResultHistory *history);

#ifdef __cplusplus
}
#endif
#endif
