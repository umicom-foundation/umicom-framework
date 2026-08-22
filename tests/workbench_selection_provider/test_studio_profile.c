/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_studio_profile.c
 *
 * PURPOSE:
 *   Verify standard Studio model providers use panel-current routing rather than fixed colour groups.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection_provider/studio_profile.h"

int main(void)
{
    UmiWorkbenchSelectionProviderStudioProfile profile =
        umi_workbench_selection_provider_studio_profile_default();
    assert(strcmp(profile.application_id, "org.umicom.studio") == 0);
    assert(strcmp(profile.project_panel_id, "studio.project-explorer") == 0);
    assert(strcmp(profile.problems_panel_id, "studio.problems") == 0);
    assert(strcmp(profile.source_control_panel_id, "studio.source-control") == 0);
    assert(strcmp(profile.test_panel_id, "studio.test-explorer") == 0);

    return 0;
}
