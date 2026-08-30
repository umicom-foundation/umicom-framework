/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/pipeline_state.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_PIPELINE_STATE_H
#define INCLUDE_UMICOM_DELIVERY_PIPELINE_STATE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPipelineState {
    UmiDeliveryStage stage;
    UmiEvidenceStatus status;
    uint64_t started_epoch_ms;
    uint64_t finished_epoch_ms;
    char message[UMI_DELIVERY_TEXT_CAPACITY];
} UmiPipelineState;

void umi_pipeline_state_init(UmiPipelineState *state);
UmiStatus umi_pipeline_state_begin(UmiPipelineState *state,
                                   UmiDeliveryStage stage,
                                   uint64_t started_epoch_ms);
UmiStatus umi_pipeline_state_finish(UmiPipelineState *state,
                                    UmiEvidenceStatus status,
                                    uint64_t finished_epoch_ms,
                                    const char *message);

#ifdef __cplusplus
}
#endif

#endif
