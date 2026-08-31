/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_codeguard_documentation_coverage.c
 *
 * PURPOSE:
 *   Verify documentation coverage recognises explained functions, decisions
 *   and the standard Umicom file header without changing the inspected file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/codeguard/documentation_coverage.h"

/* Provide a real Umicom-style function candidate with one explained safety decision. */
static UmiStatus umi_documentation_coverage_fixture(const char *text)
{
    /* Reject absent text before a future application attempts to display it. */
    if (text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Exercise the scanner against this deliberately well-documented source file. */
int main(void)
{
    UmiCodeGuardDocumentationPolicy policy;
    UmiCodeGuardDocumentationReport report;
    UmiCodeGuardDocumentationPolicy invalid_policy = {0};
    assert(umi_documentation_coverage_fixture("guide") == UMI_STATUS_OK);
    umi_codeguard_documentation_policy_default(&policy);
    /* A zero-sized policy cannot safely describe the current public contract. */
    assert(umi_codeguard_documentation_scan_file(
        __FILE__, &invalid_policy, &report) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_codeguard_documentation_scan_file(
        __FILE__, &policy, &report) == UMI_STATUS_OK);
    /* The standard header and this function comment should both be recognised. */
    assert(report.has_standard_file_header);
    assert(report.function_count >= 1U);
    assert(report.documented_function_count == report.function_count);
    assert(report.passes);
    return 0;
}
