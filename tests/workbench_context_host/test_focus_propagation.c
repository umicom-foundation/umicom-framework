/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_focus_propagation.c
 *
 * PURPOSE:
 *   Verify conservative focus preservation and hidden-panel reveal.
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
#include "umicom/workbench_context_host/focus_propagation.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostFocusPolicy policy =
        umi_workbench_context_host_focus_policy_default();
    UmiWorkbenchContextHostEndpoint endpoint;
    umi_workbench_context_host_endpoint_init(&endpoint, "editor");
    assert(umi_workbench_context_host_endpoint_set_identity(
        &endpoint, "editor", "studio", "Editor") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_set_group(
        &endpoint, "blue", UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW) == UMI_STATUS_OK);
    endpoint.role = UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR;
    assert(umi_workbench_context_host_focus_decide(
        &policy, &endpoint, UMI_CONTEXT_KIND_SOURCE_LOCATION,
        false, false) == UMI_WORKBENCH_CONTEXT_HOST_FOCUS_REVEAL);
    policy.preserve_user_focus = false;
    assert(umi_workbench_context_host_focus_decide(
        &policy, &endpoint, UMI_CONTEXT_KIND_SOURCE_LOCATION,
        true, false) == UMI_WORKBENCH_CONTEXT_HOST_FOCUS_ACTIVATE);

    return 0;
}
