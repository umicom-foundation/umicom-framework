/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_audit.c
 *
 * PURPOSE:
 *   Verify audit sequence allocation and outcome counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "umicom/diagnostics/audit.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAuditLog *log = NULL; UmiAuditRecord r = {0}, out;
    (void)strcpy(r.principal, "user.sammy"); (void)strcpy(r.action, "plugin.enable"); (void)strcpy(r.resource, "plugin.example"); r.outcome = UMI_AUDIT_SUCCEEDED;
    assert(umi_audit_log_create(&log) == UMI_STATUS_OK);
    assert(umi_audit_log_append(log, &r) == UMI_STATUS_OK);
    assert(umi_audit_log_at(log, 0U, &out) == UMI_STATUS_OK && out.sequence == 1U);
    assert(umi_audit_log_outcome_count(log, UMI_AUDIT_SUCCEEDED) == 1U);
    umi_audit_log_destroy(log); return 0;
}
