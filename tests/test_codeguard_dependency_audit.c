/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_dependency_audit.c
 *
 * PURPOSE:
 *   Verify dependency-audit defaults, report formats and honest handling of a
 *   missing optional vulnerability scanner without starting a child process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/codeguard/dependency_audit.h"

#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCodeGuardDependencyAuditRequest request;
    UmiCodeGuardDependencyAuditReport report;
    UmiCodeGuardDependencyReportFormat format;
    UmiToolchainProfile profile;
    UmiToolInfo *scanner;

    umi_toolchain_profile_init(&profile);
    umi_codeguard_dependency_audit_request_init(&request);
    /* Apply this branch only when its contract condition is satisfied. */
    if (request.format != UMI_CODEGUARD_DEPENDENCY_FORMAT_VERTICAL ||
        request.timeout_ms != 900000U || !request.recursive)
        return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_codeguard_dependency_report_format_parse("json", &format) ||
        format != UMI_CODEGUARD_DEPENDENCY_FORMAT_JSON ||
        umi_codeguard_dependency_report_format_parse("xml", &format))
        return EXIT_FAILURE;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_codeguard_dependency_audit_execute(
            &profile, &request, &report) != UMI_STATUS_OK ||
        report.outcome != UMI_CODEGUARD_DEPENDENCY_SKIPPED ||
        report.scanner_available)
        return EXIT_FAILURE;
    request.strict = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_codeguard_dependency_audit_execute(
            &profile, &request, &report) != UMI_STATUS_UNAVAILABLE ||
        report.outcome != UMI_CODEGUARD_DEPENDENCY_SKIPPED)
        return EXIT_FAILURE;

    scanner = umi_toolchain_profile_tool_mutable(
        &profile, UMI_TOOL_OSV_SCANNER);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (scanner == NULL) return EXIT_FAILURE;
    scanner->state = UMI_TOOL_VALIDATED;
    (void)strcpy(scanner->path, "not-started-during-dry-run");
    (void)strcpy(scanner->version, "test-version");
    request.dry_run = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_codeguard_dependency_audit_execute(
            &profile, &request, &report) != UMI_STATUS_OK ||
        report.outcome != UMI_CODEGUARD_DEPENDENCY_PLANNED ||
        !report.scanner_available || report.scanner_launched ||
        strcmp(report.scanner_version, "test-version") != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
