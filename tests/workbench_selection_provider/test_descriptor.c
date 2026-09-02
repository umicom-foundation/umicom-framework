/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_descriptor.c
 *
 * PURPOSE:
 *   Verify provider identity, routing and validation.
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

#include "umicom/workbench_selection_provider/descriptor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchSelectionProviderDescriptor descriptor;
    umi_workbench_selection_provider_descriptor_init(
        &descriptor, "studio.provider.project");
    assert(umi_workbench_selection_provider_descriptor_set_identity(
        &descriptor, "org.umicom.studio", "studio.project-explorer",
        "Project") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_descriptor_set_routing(
        &descriptor, "studio.project.selection", "") == UMI_STATUS_OK);
    descriptor.kind = UMI_WORKBENCH_SELECTION_PROVIDER_PROJECT;
    descriptor.state = UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE;
    descriptor.selection_kind = UMI_WORKBENCH_SELECTION_PROJECT;
    descriptor.context_kind = UMI_CONTEXT_KIND_PROJECT;
    assert(umi_workbench_selection_provider_descriptor_validate(
        &descriptor) == UMI_STATUS_OK);

    return 0;
}
