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
#include "value_archive/construction_cases.h"
#include "value_archive/construction_cases.h"
#include "umicom/delivery/pipeline.h"
#include "umicom/delivery/pipeline_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted gate value. */
static int UmiReleaseGateConstructionCases(void)
{
    UmiReleaseGate *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_release_gate_init(value, "tests", 1) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->gate_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_release_gate_init(value, oversized, 1) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_release_gate_init(value, value->gate_id, 1) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->gate_id, "tests") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}


/* A rejected replacement must preserve the previously accepted pipeline value. */
static int UmiDeliveryPipelineConstructionCases(void)
{
    UmiDeliveryPipeline *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_delivery_pipeline_init(value, "default") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->pipeline_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_pipeline_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_pipeline_init(value, value->pipeline_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->pipeline_id, "default") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDeliveryPipelineConstructionCases() != 0) return 1;

    if (UmiReleaseGateConstructionCases() != 0) return 1;

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
