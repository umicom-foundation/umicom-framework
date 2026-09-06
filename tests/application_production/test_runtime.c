/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_runtime.c
 *
 * PURPOSE:
 *   Implement the test runtime behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | runtime | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionRuntime *runtime = calloc(1U, sizeof(*runtime));
    assert(runtime != NULL);
    assert(umi_application_production_runtime_init(&adoption, umi_test_application_production_all_available, NULL, runtime) == UMI_STATUS_OK);
    assert(runtime->panels.count > 0U && runtime->layouts.count > 0U);
    /* Runtime invocation automatically records a safe dry-run in its command journal. */
    {
        UmiApplicationProductionCommandInvocationRequest request = {0};
        UmiApplicationProductionCommandInvocationResult result;
        request.structure_size = sizeof(request);
        request.kind = UMI_APPLICATION_COMMAND_SELECT_LAYOUT;
        request.target_id = runtime->binding.experience->default_layout_id;
        assert(umi_application_production_runtime_invoke_command(
                   runtime, &request, &result) == UMI_STATUS_NOT_IMPLEMENTED);
        assert(runtime->command_journal.count == 1U);
        assert(strcmp(runtime->command_journal.entries[0].target_id,
                      request.target_id) == 0);
        {
            UmiApplicationProductionCommandJournal checkpoint;
            uint64_t revision = runtime->revision;
            assert(umi_application_production_runtime_command_journal_capture(
                       runtime, &checkpoint) == UMI_STATUS_OK);
            assert(umi_application_production_runtime_command_journal_restore(
                       runtime, &checkpoint) == UMI_STATUS_OK);
            assert(runtime->revision == revision + 1U);
        }
    }
    assert(umi_application_production_runtime_checkpoint(runtime, "test", 1) == UMI_STATUS_OK);
    free(runtime);
    return 0;
}
