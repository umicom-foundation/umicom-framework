/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_workspace_recovery.c
 *
 * PURPOSE:
 *   Implement the test workspace recovery behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | workspace_recovery | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationWorkspaceRuntime source, restored;
    UmiApplicationProductionCheckpointStore store;
    UmiApplicationProductionRecoveryReport report;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_workspace_runtime_init(binding.experience, &source) == UMI_STATUS_OK);
    umi_application_production_checkpoint_store_init(&store);
    assert(umi_application_production_checkpoint_store_put(&store, &source, "crash-recovery", 0) == UMI_STATUS_OK);
    assert(umi_application_production_workspace_recover(&store, binding.experience, &restored, &report) == UMI_STATUS_OK);
    assert(report.restored);
    assert(restored.session.active_panel_count == source.session.active_panel_count);
    return 0;
}

