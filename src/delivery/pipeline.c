/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/pipeline.c
 *
 * PURPOSE:
 *   Coordinate an ordered set of release gates across the complete delivery lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The pipeline keeps source, build, test, package, verification, publishing and health checks in one deterministic sequence.
 */

#include "umicom/delivery/pipeline.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_delivery_pipeline_init(UmiDeliveryPipeline *pipeline,
                                     const char *pipeline_id)
{
    if (pipeline == NULL || pipeline_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(pipeline, 0, sizeof(*pipeline));
    return umi_delivery_copy_text(pipeline->pipeline_id,
                                  sizeof(pipeline->pipeline_id),
                                  pipeline_id);
}

UmiStatus umi_delivery_pipeline_add_gate(UmiDeliveryPipeline *pipeline,
                                         const UmiReleaseGate *gate)
{
    if (pipeline == NULL || gate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pipeline->gate_count >= UMI_DELIVERY_MAX_CHECKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    pipeline->gates[pipeline->gate_count++] = *gate;
    return UMI_STATUS_OK;
}

int umi_delivery_pipeline_passed(const UmiDeliveryPipeline *pipeline)
{
    size_t i;
    if (pipeline == NULL || pipeline->gate_count == 0U) return 0;
    for (i = 0U; i < pipeline->gate_count; ++i) {
        if (!umi_release_gate_passed(&pipeline->gates[i])) return 0;
    }
    return 1;
}

size_t umi_delivery_pipeline_failed_count(const UmiDeliveryPipeline *pipeline)
{
    size_t i;
    size_t failures = 0U;
    if (pipeline == NULL) return 0U;
    for (i = 0U; i < pipeline->gate_count; ++i) {
        if (!umi_release_gate_passed(&pipeline->gates[i])) ++failures;
    }
    return failures;
}
