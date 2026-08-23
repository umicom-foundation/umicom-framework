/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/process_tool_support.c
 *
 * PURPOSE:
 *   Parse a bounded program/arguments/cwd/timeout tool call and execute it
 *   through UmiDeveloperExecutor rather than directly launching a shell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "process_tool_support.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_coding_tool_invoke_process_kind(
    UmiAiCodingToolEnvironment *environment,
    UmiDeveloperOperationKind kind,
    const char *operation_prefix,
    const char *title,
    const char *arguments_json,
    char *output,
    size_t output_capacity)
{
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    UmiDeveloperExecutionResult result;
    char program[UMI_DEVELOPER_PATH_CAPACITY];
    char arguments[UMI_AI_CODING_TOOL_TEXT_CAPACITY];
    char working_directory[UMI_DEVELOPER_PATH_CAPACITY];
    char operation_id[UMI_DEVELOPER_ID_CAPACITY];
    uint64_t timeout = 120000U;
    UmiStatus status;

    if (environment == NULL || operation_prefix == NULL ||
        title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_tool_json_parse_object(
        arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_required_string(
            &document, "program", program, sizeof(program));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_string(
            &document, "arguments", "", arguments, sizeof(arguments));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_string(
            &document,
            "workingDirectory",
            environment->workspace_root,
            working_directory,
            sizeof(working_directory));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "timeoutMs", 120000U, &timeout);
    }
    if (status != UMI_STATUS_OK) return status;

    if (timeout > UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)snprintf(
        operation_id,
        sizeof(operation_id),
        "%s.%llu",
        operation_prefix,
        (unsigned long long)(environment->revision + 1U));

    status = umi_ai_coding_tool_execute_program(
        environment,
        kind,
        operation_id,
        title,
        program,
        arguments,
        working_directory,
        (uint32_t)timeout,
        &result);
    if (status != UMI_STATUS_OK && !result.launched) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"launched\":");
    (void)umi_language_runtime_json_writer_bool(&writer, result.launched);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"exitCode\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer,
        result.exit_code >= 0 ? (uint64_t)result.exit_code : 0U);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"timedOut\":");
    (void)umi_language_runtime_json_writer_bool(&writer, result.timed_out);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"cancelled\":");
    (void)umi_language_runtime_json_writer_bool(&writer, result.cancelled);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"durationMs\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, result.duration_ms);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"output\":");
    (void)umi_language_runtime_json_writer_string(&writer, result.output);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");

    return result.launched &&
           result.exit_code == 0 &&
           !result.timed_out &&
           !result.cancelled
        ? writer.status
        : UMI_STATUS_UNAVAILABLE;
}
