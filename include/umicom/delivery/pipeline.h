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

/**
 * Represent the delivery pipeline data shared with callers of this public contract.
 */
typedef struct UmiDeliveryPipeline {
    char pipeline_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseGate gates[UMI_DELIVERY_MAX_CHECKS];
    size_t gate_count;
} UmiDeliveryPipeline;

/**
 * Initialise delivery pipeline from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_pipeline_init(UmiDeliveryPipeline *pipeline,
                                     const char *pipeline_id);
/**
 * Provide the delivery pipeline add gate operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_pipeline_add_gate(UmiDeliveryPipeline *pipeline,
                                         const UmiReleaseGate *gate);
/**
 * Provide the delivery pipeline passed operation used by this module and its client
 * applications.
 */
int umi_delivery_pipeline_passed(const UmiDeliveryPipeline *pipeline);
/**
 * Return the number of records represented by delivery pipeline failed without changing
 * their state.
 */
size_t umi_delivery_pipeline_failed_count(const UmiDeliveryPipeline *pipeline);

#ifdef __cplusplus
}
#endif

#endif
