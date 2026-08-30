/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/dependency_audit.c
 *
 * PURPOSE:
 *   Run an optional OSV-Scanner source audit through Framework process and
 *   toolchain contracts while preserving honest skipped and failed outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/codeguard/dependency_audit.h"

#include <stdio.h>
#include <string.h>

void umi_codeguard_dependency_audit_request_init(
    UmiCodeGuardDependencyAuditRequest *request)
{
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->project_root = ".";
    request->format = UMI_CODEGUARD_DEPENDENCY_FORMAT_VERTICAL;
    request->timeout_ms = 900000U;
    request->recursive = 1;
}

void umi_codeguard_dependency_audit_report_init(
    UmiCodeGuardDependencyAuditReport *report)
{
    if (report == NULL) return;
    (void)memset(report, 0, sizeof(*report));
    report->outcome = UMI_CODEGUARD_DEPENDENCY_NOT_RUN;
}

const char *umi_codeguard_dependency_report_format_text(
    UmiCodeGuardDependencyReportFormat format)
{
    switch (format) {
        case UMI_CODEGUARD_DEPENDENCY_FORMAT_VERTICAL: return "vertical";
        case UMI_CODEGUARD_DEPENDENCY_FORMAT_JSON: return "json";
        case UMI_CODEGUARD_DEPENDENCY_FORMAT_SARIF: return "sarif";
        default: return "unknown";
    }
}

int umi_codeguard_dependency_report_format_parse(
    const char *text,
    UmiCodeGuardDependencyReportFormat *out_format)
{
    if (text == NULL || out_format == NULL) return 0;
    if (strcmp(text, "vertical") == 0)
        *out_format = UMI_CODEGUARD_DEPENDENCY_FORMAT_VERTICAL;
    else if (strcmp(text, "json") == 0)
        *out_format = UMI_CODEGUARD_DEPENDENCY_FORMAT_JSON;
    else if (strcmp(text, "sarif") == 0)
        *out_format = UMI_CODEGUARD_DEPENDENCY_FORMAT_SARIF;
    else
        return 0;
    return 1;
}

const char *umi_codeguard_dependency_audit_outcome_text(
    UmiCodeGuardDependencyAuditOutcome outcome)
{
    switch (outcome) {
        case UMI_CODEGUARD_DEPENDENCY_PLANNED: return "planned";
        case UMI_CODEGUARD_DEPENDENCY_SKIPPED: return "skipped";
        case UMI_CODEGUARD_DEPENDENCY_PASSED: return "passed";
        case UMI_CODEGUARD_DEPENDENCY_FAILED: return "failed";
        default: return "not-run";
    }
}

static void dependency_copy_text(char *destination,
                                 size_t capacity,
                                 const char *source)
{
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

UmiStatus umi_codeguard_dependency_audit_execute(
    const UmiToolchainProfile *profile,
    const UmiCodeGuardDependencyAuditRequest *request,
    UmiCodeGuardDependencyAuditReport *out_report)
{
    const UmiToolInfo *scanner;
    const char *arguments[6];
    char format_argument[32];
    size_t argument_count = 0U;
    UmiProcessRequest process_request;
    UmiProcessResult process_result;
    UmiStatus status;

    if (profile == NULL || request == NULL || out_report == NULL ||
        request->project_root == NULL || request->project_root[0] == '\0' ||
        request->format < UMI_CODEGUARD_DEPENDENCY_FORMAT_VERTICAL ||
        request->format > UMI_CODEGUARD_DEPENDENCY_FORMAT_SARIF ||
        request->environment_count > UMI_PROCESS_MAX_ENVIRONMENT ||
        (request->environment_count > 0U && request->environment == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;

    umi_codeguard_dependency_audit_report_init(out_report);
    scanner = umi_toolchain_profile_tool(profile, UMI_TOOL_OSV_SCANNER);
    if (scanner != NULL && scanner->state == UMI_TOOL_VALIDATED) {
        out_report->scanner_available = 1;
        dependency_copy_text(out_report->scanner_path,
                             sizeof(out_report->scanner_path), scanner->path);
        dependency_copy_text(out_report->scanner_version,
                             sizeof(out_report->scanner_version),
                             scanner->version);
    }
    if (request->dry_run) {
        out_report->outcome = UMI_CODEGUARD_DEPENDENCY_PLANNED;
        return UMI_STATUS_OK;
    }
    if (!out_report->scanner_available) {
        out_report->outcome = UMI_CODEGUARD_DEPENDENCY_SKIPPED;
        return request->strict ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
    }

    if (snprintf(format_argument, sizeof(format_argument), "--format=%s",
                 umi_codeguard_dependency_report_format_text(
                     request->format)) < 0)
        return UMI_STATUS_INTERNAL_ERROR;
    arguments[argument_count++] = "scan";
    arguments[argument_count++] = "source";
    arguments[argument_count++] = format_argument;
    arguments[argument_count++] = "--verbosity=error";
    if (request->recursive) arguments[argument_count++] = "--recursive";
    arguments[argument_count++] = ".";

    (void)memset(&process_request, 0, sizeof(process_request));
    (void)memset(&process_result, 0, sizeof(process_result));
    process_request.program = scanner->path;
    process_request.arguments = arguments;
    process_request.argument_count = argument_count;
    process_request.working_directory = request->project_root;
    process_request.environment = request->environment;
    process_request.environment_count = request->environment_count;
    process_request.capture_stdout = 1;
    process_request.capture_stderr = 1;
    process_request.timeout_ms = request->timeout_ms;
    process_request.poll_interval_ms = 20U;
    process_request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    status = umi_process_execute(&process_request, &process_result);

    out_report->scanner_launched = process_result.launched;
    out_report->output_truncated = process_result.output_truncated;
    out_report->exit_code = process_result.exit_code;
    dependency_copy_text(out_report->output, sizeof(out_report->output),
                         process_result.output);
    if (status != UMI_STATUS_OK || process_result.exit_code != 0) {
        out_report->outcome = UMI_CODEGUARD_DEPENDENCY_FAILED;
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
    }
    out_report->outcome = UMI_CODEGUARD_DEPENDENCY_PASSED;
    return UMI_STATUS_OK;
}
