/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/pipeline_state.c
 *
 * PURPOSE:
 *   Track the current stage, status and timestamps of one delivery pipeline execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Separating pipeline definition from runtime state makes it possible to resume, inspect and audit one release run.
 */

#include "umicom/delivery/pipeline_state.h"
#include "delivery_internal.h"
#include <string.h>

void umi_pipeline_state_init(UmiPipelineState *state)
{
    if (state != NULL) (void)memset(state, 0, sizeof(*state));
}
UmiStatus umi_pipeline_state_begin(UmiPipelineState *state,
                                   UmiDeliveryStage stage,
                                   uint64_t started_epoch_ms)
{
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    state->stage = stage;
    state->status = UMI_EVIDENCE_UNKNOWN;
    state->started_epoch_ms = started_epoch_ms;
    state->finished_epoch_ms = 0U;
    state->message[0] = '\0';
    return UMI_STATUS_OK;
}
UmiStatus umi_pipeline_state_finish(UmiPipelineState *state,
                                    UmiEvidenceStatus status,
                                    uint64_t finished_epoch_ms,
                                    const char *message)
{
    if (state == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    state->status = status;
    state->finished_epoch_ms = finished_epoch_ms;
    return umi_delivery_copy_text(state->message, sizeof(state->message), message);
}
