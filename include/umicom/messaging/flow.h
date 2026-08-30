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
typedef struct UmiFlow UmiFlow;

UmiStatus umi_flow_create(const char *flow_id,
                          size_t capacity,
                          UmiFlow **out_flow);
void umi_flow_destroy(UmiFlow *flow);
UmiStatus umi_flow_add_stage(UmiFlow *flow,
                             const char *stage_id,
                             UmiFlowStageHandler handler,
                             void *user_data);
UmiStatus umi_flow_execute(const UmiFlow *flow,
                           const UmiMessageEnvelope *input,
                           UmiOwnedMessage *output,
                           size_t *out_completed_stages);
const char *umi_flow_id(const UmiFlow *flow);
size_t umi_flow_stage_count(const UmiFlow *flow);

#ifdef __cplusplus
}
#endif

#endif
