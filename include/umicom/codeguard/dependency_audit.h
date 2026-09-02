/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/dependency_audit.h
 *
 * PURPOSE:
 *   Define a reusable dependency-vulnerability audit contract for the Umicom
 *   command, Studio Security Centre and future Framework-based applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_DEPENDENCY_AUDIT_H
#define UMICOM_CODEGUARD_DEPENDENCY_AUDIT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These values describe the result without pretending that a skipped scan
 * passed. That distinction is important in release dashboards and CI gates. */
typedef enum UmiCodeGuardDependencyAuditOutcome {
    UMI_CODEGUARD_DEPENDENCY_NOT_RUN = 0,
    UMI_CODEGUARD_DEPENDENCY_PLANNED = 1,
    UMI_CODEGUARD_DEPENDENCY_SKIPPED = 2,
    UMI_CODEGUARD_DEPENDENCY_PASSED = 3,
    UMI_CODEGUARD_DEPENDENCY_FAILED = 4
} UmiCodeGuardDependencyAuditOutcome;

/**
 * List the named code guard dependency report format values accepted by this public
 * contract.
 */
typedef enum UmiCodeGuardDependencyReportFormat {
    UMI_CODEGUARD_DEPENDENCY_FORMAT_VERTICAL = 0,
    UMI_CODEGUARD_DEPENDENCY_FORMAT_JSON = 1,
    UMI_CODEGUARD_DEPENDENCY_FORMAT_SARIF = 2
} UmiCodeGuardDependencyReportFormat;

/**
 * Represent the code guard dependency audit request data shared with callers of this
 * public contract.
 */
typedef struct UmiCodeGuardDependencyAuditRequest {
    const char *project_root;
    UmiCodeGuardDependencyReportFormat format;
    const UmiEnvironmentVariable *environment;
    size_t environment_count;
    uint32_t timeout_ms;
    int recursive;
    int strict;
    int dry_run;
} UmiCodeGuardDependencyAuditRequest;

/**
 * Represent the code guard dependency audit report data shared with callers of this public
 * contract.
 */
typedef struct UmiCodeGuardDependencyAuditReport {
    UmiCodeGuardDependencyAuditOutcome outcome;
    int scanner_available;
    int scanner_launched;
    int output_truncated;
    int exit_code;
    char scanner_path[UMI_TOOL_PATH_CAPACITY];
    char scanner_version[UMI_TOOL_VERSION_CAPACITY];
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
} UmiCodeGuardDependencyAuditReport;

/**
 * Initialise codeguard dependency audit request from caller-provided values so later
 * operations receive a known state.
 */
void umi_codeguard_dependency_audit_request_init(
    UmiCodeGuardDependencyAuditRequest *request);
/**
 * Initialise codeguard dependency audit report from caller-provided values so later
 * operations receive a known state.
 */
void umi_codeguard_dependency_audit_report_init(
    UmiCodeGuardDependencyAuditReport *report);
/**
 * Provide the codeguard dependency report format text operation used by this module and
 * its client applications.
 */
const char *umi_codeguard_dependency_report_format_text(
    UmiCodeGuardDependencyReportFormat format);
/**
 * Read codeguard dependency report format into validated module state and return a status
 * when input cannot be used.
 */
int umi_codeguard_dependency_report_format_parse(
    const char *text,
    UmiCodeGuardDependencyReportFormat *out_format);
/**
 * Provide the codeguard dependency audit outcome text operation used by this module and
 * its client applications.
 */
const char *umi_codeguard_dependency_audit_outcome_text(
    UmiCodeGuardDependencyAuditOutcome outcome);
/**
 * Perform codeguard dependency audit through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_codeguard_dependency_audit_execute(
    const UmiToolchainProfile *profile,
    const UmiCodeGuardDependencyAuditRequest *request,
    UmiCodeGuardDependencyAuditReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
