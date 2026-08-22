/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_audit.c
 *
 * PURPOSE:
 *   Verify context-host audit evidence retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/audit.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostAuditLog log;
    UmiWorkbenchContextHostAuditEntry entry;
    memset(&entry, 0, sizeof(entry));
    umi_workbench_context_host_audit_log_init(&log);
    assert(umi_workbench_context_host_copy_text(
        entry.audit_id, sizeof(entry.audit_id), "audit") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        entry.action, sizeof(entry.action), "publish") == UMI_STATUS_OK);
    entry.status = UMI_STATUS_OK;
    assert(umi_workbench_context_host_audit_log_append(
        &log, &entry) == UMI_STATUS_OK);
    assert(log.count == 1U);
    umi_workbench_context_host_audit_log_destroy(&log);

    return 0;
}
