/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tool_support.c
 *
 * PURPOSE:
 *   Implement shared safe-path, JSON output and DeveloperExecutor helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "tool_support.h"

#include <string.h>

#include "umicom/ai_coding_runtime/path.h"
#include "umicom/language_runtime/arguments.h"

/*
 * Provide the ai coding tool safe path operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_safe_path(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    char *out_path,
    size_t capacity)
{
    char requested[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    UmiStatus status;

    status = umi_ai_coding_tool_json_required_string(
        document,
        key,
        requested,
        sizeof(requested));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_runtime_path_normalize_relative(
        requested,
        out_path,
        capacity);
}

/*
 * Provide the ai coding tool write ok begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_write_ok_begin(
    UmiLanguageRuntimeJsonWriter *writer,
    char *output,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (writer == NULL || output == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(writer, output, capacity);
    return umi_language_runtime_json_writer_raw(writer, "{\"ok\":true");
}

/*
 * Provide the ai coding tool write status end operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_write_status_end(
    UmiLanguageRuntimeJsonWriter *writer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (writer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_language_runtime_json_writer_raw(writer, "}");
    return writer->status;
}

/*
 * Provide the ai coding tool execute program operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_execute_program(
    UmiAiCodingToolEnvironment *environment,
    UmiDeveloperOperationKind kind,
    const char *operation_id,
    const char *title,
    const char *program,
    const char *arguments,
    const char *working_directory,
    uint32_t timeout_ms,
    UmiDeveloperExecutionResult *out_result)
{
    UmiDeveloperOperationSnapshot operation;
    UmiLanguageRuntimeArguments parsed;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || environment->executor == NULL ||
        environment->executor->execute == NULL ||
        operation_id == NULL || title == NULL ||
        program == NULL || program[0] == '\0' ||
        arguments == NULL || working_directory == NULL ||
        out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&operation, 0, sizeof(operation));
    (void)memset(out_result, 0, sizeof(*out_result));

    status = umi_developer_operation_init(
        &operation,
        operation_id,
        kind,
        title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_operation_set_program(
        &operation,
        program,
        working_directory[0] != '\0'
            ? working_directory
            : environment->workspace_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_language_runtime_arguments_parse(arguments, &parsed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < parsed.count; ++index) {
        status = umi_developer_operation_add_argument(
            &operation,
            parsed.values[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    operation.timeout_ms = timeout_ms;

    return environment->executor->execute(
        environment->executor->user_data,
        &operation,
        out_result);
}
