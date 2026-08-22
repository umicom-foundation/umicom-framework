/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_types.c
 *
 * PURPOSE:
 *   Verify bounded text, context masks and enum text contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

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
    return 0;
}
