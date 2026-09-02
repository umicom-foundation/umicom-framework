/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_pipeline.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/pipeline.h"
#include "umicom/delivery/pipeline_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDeliveryPipeline pipeline;
    UmiReleaseGate gate;
    UmiPipelineState state;
    assert(umi_delivery_pipeline_init(&pipeline, "default") == UMI_STATUS_OK);
    assert(umi_release_gate_init(&gate, "tests", 1) == UMI_STATUS_OK);
    assert(umi_release_gate_record(&gate, UMI_EVIDENCE_PASS, "all passed") == UMI_STATUS_OK);
    assert(umi_delivery_pipeline_add_gate(&pipeline, &gate) == UMI_STATUS_OK);
    assert(umi_delivery_pipeline_passed(&pipeline));
    umi_pipeline_state_init(&state);
    assert(umi_pipeline_state_begin(&state, UMI_DELIVERY_STAGE_TEST, 1U) == UMI_STATUS_OK);
    assert(umi_pipeline_state_finish(&state, UMI_EVIDENCE_PASS, 2U, "ok") == UMI_STATUS_OK);
    return 0;
}
