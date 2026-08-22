/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_group_catalogue.c
 *
 * PURPOSE:
 *   Verify deterministic group catalogue insertion, replacement and removal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/workbench_context_link/group_catalogue.h"

int main(void)
{
    UmiWorkbenchContextLinkGroupCatalogue catalogue;
    UmiWorkbenchContextLinkGroupProfile profile;
    umi_workbench_context_link_group_catalogue_init(&catalogue);
    umi_workbench_context_link_group_profile_init(&profile, "blue");
    assert(umi_workbench_context_link_group_profile_set_primary(&profile, "Blue") == UMI_STATUS_OK);
    profile.flags = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    assert(umi_workbench_context_link_group_catalogue_upsert(&catalogue, &profile) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_group_catalogue_count(&catalogue) == 1U);
    assert(umi_workbench_context_link_group_catalogue_find_const(&catalogue, "blue") != NULL);
    assert(umi_workbench_context_link_group_catalogue_remove(&catalogue, "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_group_catalogue_count(&catalogue) == 0U);
    umi_workbench_context_link_group_catalogue_destroy(&catalogue);
    return 0;
}
