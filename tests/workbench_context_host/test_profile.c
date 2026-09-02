/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_profile.c
 *
 * PURPOSE:
 *   Verify bounded group and endpoint profile composition.
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

    UmiWorkbenchContextHostProfile profile;
    UmiWorkbenchContextHostGroupDefinition group;
    UmiWorkbenchContextHostEndpoint endpoint;
    umi_workbench_context_host_profile_init(
        &profile, "profile", "org.umicom.test");
    assert(umi_workbench_context_host_profile_set_title(
        &profile, "Profile") == UMI_STATUS_OK);
    umi_workbench_context_host_group_definition_init(&group, "blue");
    assert(umi_workbench_context_host_copy_text(
        group.title, sizeof(group.title), "Blue") == UMI_STATUS_OK);
    group.colour = UMI_CONTEXT_COLOUR_BLUE;
    assert(umi_workbench_context_host_profile_add_group(
        &profile, &group) == UMI_STATUS_OK);
    umi_workbench_context_host_endpoint_init(&endpoint, "endpoint");
    assert(umi_workbench_context_host_endpoint_set_identity(
        &endpoint, "panel", "org.umicom.test", "Panel") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_endpoint_set_group(
        &endpoint, "blue", UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_profile_add_endpoint(
        &profile, &endpoint) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_profile_validate(&profile) == UMI_STATUS_OK);
    assert(strcmp(
        umi_workbench_context_host_profile_default_group(&profile),
        "blue") == 0);
    return 0;
}
