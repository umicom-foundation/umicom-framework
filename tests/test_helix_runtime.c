/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_helix_runtime.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include "umicom/umicom.h"

int main(void)
{
    UmiHelixRuntime runtime;
    umi_helix_runtime_init(&runtime, "test");
    assert(umi_helix_runtime_register_default_agents(&runtime, "test-provider") == UMI_STATUS_OK);
    assert(runtime.agents.count == 6U);
    assert(umi_helix_workflow_advance(&runtime.workflow, UMI_HELIX_STAGE_DIAGNOSE) == UMI_STATUS_OK);
    assert(runtime.workflow.stage == UMI_HELIX_STAGE_DIAGNOSE);
    return 0;
}
