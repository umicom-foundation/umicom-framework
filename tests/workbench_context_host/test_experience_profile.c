/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_experience_profile.c
 *
 * PURPOSE:
 *   Verify a canonical application experience becomes a complete, valid
 *   linked-workbench profile with active and passive panel endpoints.
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

#include "umicom/workbench_context_host/experience_profile.h"

/*
 * Exercise find endpoint and return a clear result when the behaviour no longer matches
 * its contract.
 */
static const UmiWorkbenchContextHostEndpoint *find_endpoint(
    const UmiWorkbenchContextHostProfile *profile,
    const char *panel_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->endpoint_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(profile->endpoints[index].panel_id, panel_id) == 0) {
            return &profile->endpoints[index];
        }
    }
    return NULL;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostProfile profile;
    const UmiWorkbenchContextHostEndpoint *canvas;
    const UmiWorkbenchContextHostEndpoint *network;

    assert(umi_workbench_context_host_experience_profile_build_for_application(
        "org.umicom.web-studio", NULL, &profile) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_profile_validate(&profile) ==
           UMI_STATUS_OK);
    assert(strcmp(profile.profile_id,
                  "org.umicom.web-studio.experience-context") == 0);
    assert(strcmp(profile.title, "Umicom Web Studio") == 0);
    assert(profile.group_count == 2U);
    assert(profile.endpoint_count == 9U);

    canvas = find_endpoint(&profile, "canvas");
    network = find_endpoint(&profile, "network");
    assert(canvas != NULL);
    assert(canvas->mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL);
    assert(canvas->published_kinds_mask != 0U);
    assert(network != NULL);
    assert(network->mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW);
    assert(network->published_kinds_mask == 0U);
    assert(strcmp(umi_workbench_context_host_profile_default_group(&profile),
                  "development.blue") == 0);

    return 0;
}
