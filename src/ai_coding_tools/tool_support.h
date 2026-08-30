/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tool_support.h
 *
 * PURPOSE:
 *   Internal helpers shared by built-in coding tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_TOOL_SUPPORT_H
#define UMICOM_AI_CODING_TOOLS_TOOL_SUPPORT_H

#include "umicom/ai_coding_tools/json.h"
#include "umicom/ai_coding_tools/tool_contract.h"
#include "umicom/ai_coding_runtime/scanner.h"

UmiStatus umi_ai_coding_tool_safe_path(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    char *out_path,
    size_t capacity);

UmiStatus umi_ai_coding_tool_write_ok_begin(
    UmiLanguageRuntimeJsonWriter *writer,
    char *output,
    size_t capacity);

UmiStatus umi_ai_coding_tool_write_status_end(
    UmiLanguageRuntimeJsonWriter *writer);

UmiStatus umi_ai_coding_tool_execute_program(
    UmiAiCodingToolEnvironment *environment,
    UmiDeveloperOperationKind kind,
    const char *operation_id,
    const char *title,
    const char *program,
    const char *arguments,
    const char *working_directory,
    uint32_t timeout_ms,
    UmiDeveloperExecutionResult *out_result);

#endif
