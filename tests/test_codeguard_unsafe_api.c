/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_unsafe_api.c
 *
 * PURPOSE:
 *   Implement the test codeguard unsafe api behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Unsafe API v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/codeguard/unsafe_api.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCodeGuardEvidenceStore evidence = {0};
    assert(umi_codeguard_unsafe_api_rule_count() >= 6U);
    assert(umi_codeguard_unsafe_api_scan_line("src/legacy.c",42U,"sprintf(buffer, \"%s\", value);",&evidence) == UMI_STATUS_OK);
    assert(evidence.count == 1U && evidence.items[0].state == UMI_CODEGUARD_EVIDENCE_FAIL);
    assert(strstr(evidence.items[0].remediation,"snprintf") != NULL);
    return 0;
}
