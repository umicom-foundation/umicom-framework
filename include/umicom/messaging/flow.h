/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/flow.h
 *
 * PURPOSE:
 *   Declare executable Integration Fabric flows composed from named bounded stages with one owned message passed between stages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_FLOW_H
#define UMICOM_MESSAGING_FLOW_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiFlowStageHandler)(const UmiMessageEnvelope *input,
                                         UmiOwnedMessage *output,
                                         void *user_data);
/**
 * Represent the flow data shared with callers of this public contract.
 */
typedef struct UmiFlow UmiFlow;

/**
 * Initialise flow from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_flow_create(const char *flow_id,
                          size_t capacity,
                          UmiFlow **out_flow);
/**
 * Release or reset state held by flow so the same storage can be reused safely.
 */
void umi_flow_destroy(UmiFlow *flow);
/**
 * Provide the flow add stage operation used by this module and its client applications.
 */
UmiStatus umi_flow_add_stage(UmiFlow *flow,
                             const char *stage_id,
                             UmiFlowStageHandler handler,
                             void *user_data);
/**
 * Perform flow through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_flow_execute(const UmiFlow *flow,
                           const UmiMessageEnvelope *input,
                           UmiOwnedMessage *output,
                           size_t *out_completed_stages);
/**
 * Provide the flow id operation used by this module and its client applications.
 */
const char *umi_flow_id(const UmiFlow *flow);
/**
 * Return the number of records represented by flow stage without changing their state.
 */
size_t umi_flow_stage_count(const UmiFlow *flow);

#ifdef __cplusplus
}
#endif

#endif
