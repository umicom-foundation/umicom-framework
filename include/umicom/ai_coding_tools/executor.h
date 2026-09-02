/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/executor.h
 *
 * PURPOSE:
 *   Execute model-requested tools only after descriptor resolution and central
 *   capability/risk/approval policy checks, then record bounded result history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_EXECUTOR_H
#define UMICOM_AI_CODING_TOOLS_EXECUTOR_H

#include "umicom/ai_coding_tools/catalogue.h"
#include "umicom/ai_coding_tools/result_history.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding tool executor data shared with callers of this public contract.
 */
typedef struct UmiAiCodingToolExecutor {
    UmiAiCodingToolEnvironment *environment;
    UmiAiCodingToolResultHistory *history;
    uint64_t sequence;
} UmiAiCodingToolExecutor;

/**
 * Initialise ai coding tool executor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_tool_executor_init(
    UmiAiCodingToolExecutor *executor,
    UmiAiCodingToolEnvironment *environment);

/**
 * Provide the ai coding tool executor deinit operation used by this module and its client
 * applications.
 */
void umi_ai_coding_tool_executor_deinit(
    UmiAiCodingToolExecutor *executor);

/**
 * Perform ai coding tool through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_coding_tool_execute(
    UmiAiCodingToolExecutor *executor,
    const UmiAiCodingToolCall *call,
    UmiAiCodingToolResult *out_result);

/**
 * Provide the ai coding tool executor history operation used by this module and its client
 * applications.
 */
UmiAiCodingToolResultHistory *umi_ai_coding_tool_executor_history(
    UmiAiCodingToolExecutor *executor);

#ifdef __cplusplus
}
#endif
#endif
