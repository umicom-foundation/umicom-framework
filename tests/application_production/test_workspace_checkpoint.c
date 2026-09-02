/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_workspace_checkpoint.c
 *
 * PURPOSE:
 *   Implement the test workspace checkpoint behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | workspace_checkpoint | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationWorkspaceRuntime workspace;
    UmiApplicationProductionWorkspaceCheckpoint checkpoint;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_workspace_runtime_init(binding.experience, &workspace) == UMI_STATUS_OK);
    assert(umi_application_production_workspace_checkpoint_capture(&workspace, 1U, "manual-save", 0, &checkpoint) == UMI_STATUS_OK);
    assert(checkpoint.session.active_panel_count > 0U);
    return 0;
}

