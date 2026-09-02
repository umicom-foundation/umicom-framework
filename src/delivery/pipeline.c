/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/pipeline.c
 *
 * PURPOSE:
 *   Coordinate an ordered set of release gates across the complete delivery lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The pipeline keeps source, build, test, package, verification, publishing and health checks in one deterministic sequence.
 */

#include "umicom/delivery/pipeline.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise delivery pipeline from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_pipeline_init(UmiDeliveryPipeline *pipeline,
                                     const char *pipeline_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || pipeline_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(pipeline, 0, sizeof(*pipeline));
    return umi_delivery_copy_text(pipeline->pipeline_id,
                                  sizeof(pipeline->pipeline_id),
                                  pipeline_id);
}

/*
 * Provide the delivery pipeline add gate operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_pipeline_add_gate(UmiDeliveryPipeline *pipeline,
                                         const UmiReleaseGate *gate)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || gate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (pipeline->gate_count >= UMI_DELIVERY_MAX_CHECKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    pipeline->gates[pipeline->gate_count++] = *gate;
    return UMI_STATUS_OK;
}

/*
 * Provide the delivery pipeline passed operation used by this module and its client
 * applications.
 */
int umi_delivery_pipeline_passed(const UmiDeliveryPipeline *pipeline)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || pipeline->gate_count == 0U) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < pipeline->gate_count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_release_gate_passed(&pipeline->gates[i])) return 0;
    }
    return 1;
}

/*
 * Return the number of records represented by delivery pipeline failed without changing
 * their state.
 */
size_t umi_delivery_pipeline_failed_count(const UmiDeliveryPipeline *pipeline)
{
    size_t i;
    size_t failures = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < pipeline->gate_count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_release_gate_passed(&pipeline->gates[i])) ++failures;
    }
    return failures;
}
