/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_scope_policy.c
 *
 * PURPOSE:
 *   Verify workspace and application scope constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/scope_policy.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostScopePolicy policy;
    UmiContextPayload payload;
    umi_workbench_context_host_scope_policy_init(&policy);
    assert(umi_wch_test_selection_payload(
        &payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_context_copy_text(
        payload.audit.workspace_id, sizeof(payload.audit.workspace_id),
        "workspace") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        policy.workspace_id, sizeof(policy.workspace_id), "workspace") == UMI_STATUS_OK);
    policy.require_workspace_match = true;
    assert(umi_workbench_context_host_scope_allows(&policy, &payload));
    assert(umi_workbench_context_host_copy_text(
        policy.workspace_id, sizeof(policy.workspace_id), "other") == UMI_STATUS_OK);
    assert(!umi_workbench_context_host_scope_allows(&policy, &payload));

    return 0;
}
