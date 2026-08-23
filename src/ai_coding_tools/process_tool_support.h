/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/process_tool_support.h
 *
 * PURPOSE:
 *   Internal shared implementation for process-executing developer tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PROCESS_TOOL_SUPPORT_H
#define UMICOM_AI_CODING_TOOLS_PROCESS_TOOL_SUPPORT_H
#include "tool_support.h"

UmiStatus umi_ai_coding_tool_invoke_process_kind(
    UmiAiCodingToolEnvironment *environment,
    UmiDeveloperOperationKind kind,
    const char *operation_prefix,
    const char *title,
    const char *arguments_json,
    char *output,
    size_t output_capacity);

#endif
