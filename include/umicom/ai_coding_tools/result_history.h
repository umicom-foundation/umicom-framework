/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/result_history.h
 *
 * PURPOSE:
 *   Retain bounded tool-call results for agent reasoning, workbench history and
 *   audit without making result lifetime depend on a caller stack frame.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_RESULT_HISTORY_H
#define UMICOM_AI_CODING_TOOLS_RESULT_HISTORY_H
#include "umicom/ai_coding_tools/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding tool result history data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingToolResultHistory UmiAiCodingToolResultHistory;

/**
 * Initialise ai coding tool result history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_tool_result_history_create(
    UmiAiCodingToolResultHistory **out_history);
/**
 * Release or reset state held by ai coding tool result history so the same storage can be
 * reused safely.
 */
void umi_ai_coding_tool_result_history_destroy(
    UmiAiCodingToolResultHistory *history);
/**
 * Provide the ai coding tool result history record operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_result_history_record(
    UmiAiCodingToolResultHistory *history,
    const UmiAiCodingToolResult *result);
/**
 * Find ai coding tool result history while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_coding_tool_result_history_at(
    const UmiAiCodingToolResultHistory *history,
    size_t index,
    UmiAiCodingToolResult *out_result);
/**
 * Return the number of records represented by ai coding tool result history without
 * changing their state.
 */
size_t umi_ai_coding_tool_result_history_count(
    const UmiAiCodingToolResultHistory *history);

#ifdef __cplusplus
}
#endif
#endif
