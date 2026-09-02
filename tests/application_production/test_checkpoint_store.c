/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_checkpoint_store.c
 *
 * PURPOSE:
 *   Implement the test checkpoint store behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | checkpoint_store | Sammy Hegab | Umicom Foundation | MIT */
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
    UmiApplicationProductionCheckpointStore store;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_workspace_runtime_init(binding.experience, &workspace) == UMI_STATUS_OK);
    umi_application_production_checkpoint_store_init(&store);
    assert(umi_application_production_checkpoint_store_put(&store, &workspace, "autosave", 0) == UMI_STATUS_OK);
    assert(umi_application_production_checkpoint_store_latest(&store, "org.umicom.studio") != NULL);
    return 0;
}

