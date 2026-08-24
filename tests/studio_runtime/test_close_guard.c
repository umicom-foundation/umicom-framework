/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_close_guard.c
 *
 * PURPOSE:
 *   Verify close-guard validation and decision text contract. Full working-copy
 *   behavior remains covered by DocumentCoordinator integration tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/close_guard.h"

int main(void)
{
    UmiIdeActiveContext context = {0};
    UmiStudioRuntimeCloseReport report;

    assert(umi_studio_close_guard_evaluate(
        NULL, &context, &report) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(
        umi_studio_runtime_close_decision_text(
            UMI_STUDIO_CLOSE_CONFIRM_DIRTY_AND_RUNNING),
        "confirm-dirty-and-running") == 0);
    return 0;
}
