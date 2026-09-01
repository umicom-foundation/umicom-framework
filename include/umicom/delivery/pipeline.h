/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/pipeline.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_PIPELINE_H
#define INCLUDE_UMICOM_DELIVERY_PIPELINE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/gate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryPipeline {
    char pipeline_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseGate gates[UMI_DELIVERY_MAX_CHECKS];
    size_t gate_count;
} UmiDeliveryPipeline;

UmiStatus umi_delivery_pipeline_init(UmiDeliveryPipeline *pipeline,
                                     const char *pipeline_id);
UmiStatus umi_delivery_pipeline_add_gate(UmiDeliveryPipeline *pipeline,
                                         const UmiReleaseGate *gate);
int umi_delivery_pipeline_passed(const UmiDeliveryPipeline *pipeline);
size_t umi_delivery_pipeline_failed_count(const UmiDeliveryPipeline *pipeline);

#ifdef __cplusplus
}
#endif

#endif
