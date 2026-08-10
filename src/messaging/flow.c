/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/flow.c
 *
 * PURPOSE:
 *   Implement named sequential flow stages with owned intermediate messages, completed-stage reporting and failure cleanup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/flow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiFlowStage {
    char id[128];
    UmiFlowStageHandler handler;
    void *user_data;
} UmiFlowStage;

struct UmiFlow {
    char id[128];
    UmiFlowStage *stages;
    size_t capacity;
    size_t count;
};

UmiStatus umi_flow_create(const char *flow_id,
                          size_t capacity,
                          UmiFlow **out_flow)
{
    UmiFlow *flow;
    if (flow_id == NULL || flow_id[0] == '\0' || capacity == 0U ||
        out_flow == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_flow = NULL;
    flow = (UmiFlow *)calloc(1U, sizeof(*flow));
    if (flow == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    flow->stages = (UmiFlowStage *)calloc(capacity, sizeof(*flow->stages));
    if (flow->stages == NULL) {
        free(flow);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(flow->id, sizeof(flow->id), "%s", flow_id);
    flow->capacity = capacity;
    *out_flow = flow;
    return UMI_STATUS_OK;
}

void umi_flow_destroy(UmiFlow *flow)
{
    if (flow == NULL) return;
    free(flow->stages);
    free(flow);
}

UmiStatus umi_flow_add_stage(UmiFlow *flow,
                             const char *stage_id,
                             UmiFlowStageHandler handler,
                             void *user_data)
{
    UmiFlowStage *stage;
    if (flow == NULL || stage_id == NULL || stage_id[0] == '\0' ||
        handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (flow->count >= flow->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    stage = &flow->stages[flow->count++];
    (void)snprintf(stage->id, sizeof(stage->id), "%s", stage_id);
    stage->handler = handler;
    stage->user_data = user_data;
    return UMI_STATUS_OK;
}

UmiStatus umi_flow_execute(const UmiFlow *flow,
                           const UmiMessageEnvelope *input,
                           UmiOwnedMessage *output,
                           size_t *out_completed_stages)
{
    UmiOwnedMessage current;
    UmiOwnedMessage next;
    UmiStatus status;
    size_t index;
    if (flow == NULL || input == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out_completed_stages != NULL) *out_completed_stages = 0U;
    status = umi_message_copy(input, &current);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < flow->count; ++index) {
        (void)memset(&next, 0, sizeof(next));
        status = flow->stages[index].handler(&current.envelope,
                                             &next,
                                             flow->stages[index].user_data);
        umi_message_dispose(&current);
        if (status != UMI_STATUS_OK) {
            umi_message_dispose(&next);
            return status;
        }
        current = next;
        if (out_completed_stages != NULL) *out_completed_stages = index + 1U;
    }
    *output = current;
    return UMI_STATUS_OK;
}

const char *umi_flow_id(const UmiFlow *flow)
{
    return flow != NULL ? flow->id : "";
}

size_t umi_flow_stage_count(const UmiFlow *flow)
{
    return flow != NULL ? flow->count : 0U;
}
