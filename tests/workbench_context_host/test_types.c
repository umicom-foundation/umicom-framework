/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_types.c
 *
 * PURPOSE:
 *   Verify bounded text, context masks and enum text contracts.
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
#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    char text[16U];
    assert(umi_workbench_context_host_copy_text(
        text, sizeof(text), "context") == UMI_STATUS_OK);
    assert(strcmp(text, "context") == 0);
    assert(umi_workbench_context_host_copy_text(
        text, 4U, "context") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_workbench_context_host_kind_allowed(
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_PROJECT),
        UMI_CONTEXT_KIND_PROJECT));
    assert(!umi_workbench_context_host_kind_allowed(
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_PROJECT),
        UMI_CONTEXT_KIND_TRADE));
    assert(strcmp(
        umi_workbench_context_host_panel_role_text(
            UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR),
        "editor") == 0);
    assert(strcmp(
        umi_workbench_context_host_panel_role_text(
            UMI_WORKBENCH_CONTEXT_HOST_PANEL_PAYMENT),
        "payment") == 0);
    assert(strcmp(
        umi_workbench_context_host_panel_role_text(
            UMI_WORKBENCH_CONTEXT_HOST_PANEL_KNOWLEDGE),
        "knowledge") == 0);
    assert(strcmp(
        umi_workbench_context_host_panel_role_text(
            UMI_WORKBENCH_CONTEXT_HOST_PANEL_SYSTEM),
        "system") == 0);
    return 0;
}
