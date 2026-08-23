/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/executor.h
 *
 * PURPOSE:
 *   Execute model-requested tools only after descriptor resolution and central
 *   capability/risk/approval policy checks, then record bounded result history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_EXECUTOR_H
#define UMICOM_AI_CODING_TOOLS_EXECUTOR_H

#include "umicom/ai_coding_tools/catalogue.h"
#include "umicom/ai_coding_tools/result_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingToolExecutor {
    UmiAiCodingToolEnvironment *environment;
    UmiAiCodingToolResultHistory *history;
    uint64_t sequence;
} UmiAiCodingToolExecutor;

UmiStatus umi_ai_coding_tool_executor_init(
    UmiAiCodingToolExecutor *executor,
    UmiAiCodingToolEnvironment *environment);

void umi_ai_coding_tool_executor_deinit(
    UmiAiCodingToolExecutor *executor);

UmiStatus umi_ai_coding_tool_execute(
    UmiAiCodingToolExecutor *executor,
    const UmiAiCodingToolCall *call,
    UmiAiCodingToolResult *out_result);

UmiAiCodingToolResultHistory *umi_ai_coding_tool_executor_history(
    UmiAiCodingToolExecutor *executor);

#ifdef __cplusplus
}
#endif
#endif
