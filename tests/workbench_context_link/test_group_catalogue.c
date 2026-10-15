/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_group_catalogue.c
 *
 * PURPOSE:
 *   Verify deterministic group catalogue insertion, replacement and removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../value_archive/revision_cases.h"
#include <assert.h>
#include "umicom/workbench_context_link/group_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextLinkGroupCatalogue catalogue;
    UmiWorkbenchContextLinkGroupProfile profile;
    umi_workbench_context_link_group_catalogue_init(&catalogue);
    umi_workbench_context_link_group_profile_init(&profile, "blue");
    assert(umi_workbench_context_link_group_profile_set_primary(&profile, "Blue") == UMI_STATUS_OK);
    profile.flags = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    assert(umi_workbench_context_link_group_catalogue_upsert(&catalogue, &profile) == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiWorkbenchContextLinkGroupCatalogue, &catalogue,
        (umi_workbench_context_link_group_catalogue_upsert(&catalogue, &profile)));
    /* Replacing an existing child must not wrap its independent token. */
    {
        uint64_t input_revision = profile.revision;
        uint64_t owner_revision = catalogue.revision;
        unsigned char child_before[sizeof(catalogue.items[0])];
        memcpy(child_before, &catalogue.items[0], sizeof(child_before));
        profile.revision = UINT64_MAX;
        UmiStatus replace_status = umi_workbench_context_link_group_catalogue_upsert(&catalogue, &profile);
        profile.revision = input_revision;
        if (replace_status != UMI_STATUS_CAPACITY_EXCEEDED ||
            catalogue.revision != owner_revision ||
            memcmp(child_before, &catalogue.items[0], sizeof(child_before)) != 0)
            return 1;
    }

    assert(umi_workbench_context_link_group_catalogue_count(&catalogue) == 1U);
    assert(umi_workbench_context_link_group_catalogue_find_const(&catalogue, "blue") != NULL);
    assert(umi_workbench_context_link_group_catalogue_remove(&catalogue, "blue") == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiWorkbenchContextLinkGroupCatalogue, &catalogue,
        (umi_workbench_context_link_group_catalogue_remove(&catalogue, "blue")));

    assert(umi_workbench_context_link_group_catalogue_count(&catalogue) == 0U);
    umi_workbench_context_link_group_catalogue_destroy(&catalogue);
    return 0;
}
