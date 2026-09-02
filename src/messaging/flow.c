/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/flow.c
 *
 * PURPOSE:
 *   Implement named sequential flow stages with owned intermediate messages, completed-stage reporting and failure cleanup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Initialise flow from caller-provided values so later operations receive a known state. */
UmiStatus umi_flow_create(const char *flow_id,
                          size_t capacity,
                          UmiFlow **out_flow)
{
    UmiFlow *flow;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (flow_id == NULL || flow_id[0] == '\0' || capacity == 0U ||
        out_flow == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_flow = NULL;
    flow = (UmiFlow *)calloc(1U, sizeof(*flow));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (flow == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    flow->stages = (UmiFlowStage *)calloc(capacity, sizeof(*flow->stages));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (flow->stages == NULL) {
        free(flow);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(flow->id, sizeof(flow->id), "%s", flow_id);
    flow->capacity = capacity;
    *out_flow = flow;
    return UMI_STATUS_OK;
}

/* Release or reset state held by flow so the same storage can be reused safely. */
void umi_flow_destroy(UmiFlow *flow)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (flow == NULL) return;
    free(flow->stages);
    free(flow);
}

/* Provide the flow add stage operation used by this module and its client applications. */
UmiStatus umi_flow_add_stage(UmiFlow *flow,
                             const char *stage_id,
                             UmiFlowStageHandler handler,
                             void *user_data)
{
    UmiFlowStage *stage;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (flow == NULL || stage_id == NULL || stage_id[0] == '\0' ||
        handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (flow->count >= flow->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    stage = &flow->stages[flow->count++];
    (void)snprintf(stage->id, sizeof(stage->id), "%s", stage_id);
    stage->handler = handler;
    stage->user_data = user_data;
    return UMI_STATUS_OK;
}

/*
 * Perform flow through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_flow_execute(const UmiFlow *flow,
                           const UmiMessageEnvelope *input,
                           UmiOwnedMessage *output,
                           size_t *out_completed_stages)
{
    UmiOwnedMessage current;
    UmiOwnedMessage next;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (flow == NULL || input == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_completed_stages != NULL) *out_completed_stages = 0U;
    status = umi_message_copy(input, &current);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < flow->count; ++index) {
        (void)memset(&next, 0, sizeof(next));
        status = flow->stages[index].handler(&current.envelope,
                                             &next,
                                             flow->stages[index].user_data);
        umi_message_dispose(&current);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_message_dispose(&next);
            return status;
        }
        current = next;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_completed_stages != NULL) *out_completed_stages = index + 1U;
    }
    *output = current;
    return UMI_STATUS_OK;
}

/* Provide the flow id operation used by this module and its client applications. */
const char *umi_flow_id(const UmiFlow *flow)
{
    return flow != NULL ? flow->id : "";
}

/* Return the number of records represented by flow stage without changing their state. */
size_t umi_flow_stage_count(const UmiFlow *flow)
{
    return flow != NULL ? flow->count : 0U;
}
