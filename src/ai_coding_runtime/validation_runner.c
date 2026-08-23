/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_runner.c
 *
 * PURPOSE:
 *   Execute validation operations through the shared developer executor and
 *   collect deterministic evidence for acceptance, retry or rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_runner.h"

#include <stdio.h>
#include <string.h>

#include "umicom/language_runtime/arguments.h"

static UmiDeveloperOperationKind operation_kind(
    UmiAiCodingValidationKind kind)
{
    switch (kind) {
        case UMI_AI_CODING_VALIDATION_CONFIGURE:
            return UMI_DEVELOPER_OPERATION_CONFIGURE;
        case UMI_AI_CODING_VALIDATION_BUILD:
            return UMI_DEVELOPER_OPERATION_BUILD;
        case UMI_AI_CODING_VALIDATION_TEST:
            return UMI_DEVELOPER_OPERATION_TEST;
        case UMI_AI_CODING_VALIDATION_PACKAGE:
            return UMI_DEVELOPER_OPERATION_PACKAGE;
        case UMI_AI_CODING_VALIDATION_LINT:
        case UMI_AI_CODING_VALIDATION_FORMAT_CHECK:
        case UMI_AI_CODING_VALIDATION_CUSTOM:
        default:
            return UMI_DEVELOPER_OPERATION_CUSTOM;
    }
}

static UmiStatus execute_step(
    const UmiAiCodingValidationStep *step,
    const UmiDeveloperExecutor *executor,
    UmiAiCodingValidationResult *out_result)
{
    UmiDeveloperOperationSnapshot operation;
    UmiLanguageRuntimeArguments arguments;
    UmiDeveloperExecutionResult execution;
    size_t index;
    UmiStatus status;

    if (step == NULL || executor == NULL ||
        executor->execute == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    (void)memset(&operation, 0, sizeof(operation));
    (void)memset(&execution, 0, sizeof(execution));

    status = umi_developer_operation_init(
        &operation,
        step->id,
        operation_kind(step->kind),
        step->label[0] != '\0' ? step->label : step->id);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_operation_set_program(
        &operation,
        step->executable,
        step->working_directory[0] != '\0'
            ? step->working_directory
            : ".");
    if (status != UMI_STATUS_OK) return status;

    status = umi_language_runtime_arguments_parse(
        step->arguments,
        &arguments);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < arguments.count; ++index) {
        status = umi_developer_operation_add_argument(
            &operation,
            arguments.values[index]);
        if (status != UMI_STATUS_OK) return status;
    }

    operation.timeout_ms = step->timeout_ms;

    status = executor->execute(
        executor->user_data,
        &operation,
        &execution);

    (void)snprintf(
        out_result->step_id,
        sizeof(out_result->step_id),
        "%s",
        step->id);
    out_result->kind = step->kind;
    out_result->status = status;
    out_result->exit_code = execution.exit_code;
    out_result->passed =
        status == UMI_STATUS_OK &&
        execution.launched &&
        execution.exit_code == 0 &&
        !execution.cancelled &&
        !execution.timed_out;
    out_result->required = step->required;
    out_result->output_truncated = execution.output_truncated;
    out_result->duration_ms = execution.duration_ms;
    (void)snprintf(
        out_result->output,
        sizeof(out_result->output),
        "%s",
        execution.output);

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_validation_run(
    const UmiAiCodingValidationPlan *plan,
    const UmiDeveloperExecutor *executor,
    UmiAiCodingValidationReport *out_report)
{
    size_t index;
    UmiStatus status;

    if (plan == NULL || executor == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->passed = 1;
    out_report->revision = plan->revision;

    for (index = 0U; index < plan->step_count; ++index) {
        const UmiAiCodingValidationStep *step = &plan->steps[index];
        UmiAiCodingValidationResult *result;

        if (!step->enabled) continue;

        if (out_report->result_count >=
            UMI_AI_CODING_RUNTIME_VALIDATION_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        result = &out_report->results[out_report->result_count];

        status = execute_step(step, executor, result);
        if (status != UMI_STATUS_OK) return status;

        out_report->result_count += 1U;

        if (result->passed) {
            out_report->passed_count += 1U;
        } else {
            out_report->failed_count += 1U;
            out_report->passed = 0;

            if (result->required) {
                out_report->required_failed_count += 1U;
            }
        }
    }

    if (out_report->required_failed_count > 0U) {
        out_report->passed = 0;
    }

    return UMI_STATUS_OK;
}
