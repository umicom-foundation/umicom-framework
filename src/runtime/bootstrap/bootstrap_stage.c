/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_stage.c
 *
 * PURPOSE:
 *   Track validated bootstrap-stage state transitions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_stage.h"


#include <string.h>
UmiStatus umi_bootstrap_stage_init(UmiBootstrapStage *stage,
                                   const char *stage_id,
                                   int32_t order) {
    UmiStatus status;
    if (stage == NULL || !umi_bootstrap_id_valid(stage_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(stage, 0, sizeof(*stage));
    status = umi_bootstrap_copy_text(stage->stage_id, sizeof(stage->stage_id), stage_id);
    if (status != UMI_STATUS_OK) return status;
    stage->order = order;
    stage->state = UMI_BOOTSTRAP_STAGE_PENDING;
    return UMI_STATUS_OK;
}
UmiStatus umi_bootstrap_stage_transition(UmiBootstrapStage *stage,
                                         UmiBootstrapStageState next_state) {
    if (stage == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (stage->state) {
        case UMI_BOOTSTRAP_STAGE_PENDING:
            if (next_state != UMI_BOOTSTRAP_STAGE_READY &&
                next_state != UMI_BOOTSTRAP_STAGE_SKIPPED &&
                next_state != UMI_BOOTSTRAP_STAGE_FAILED) return UMI_STATUS_INVALID_STATE;
            break;
        case UMI_BOOTSTRAP_STAGE_READY:
            if (next_state != UMI_BOOTSTRAP_STAGE_RUNNING &&
                next_state != UMI_BOOTSTRAP_STAGE_SKIPPED &&
                next_state != UMI_BOOTSTRAP_STAGE_FAILED) return UMI_STATUS_INVALID_STATE;
            break;
        case UMI_BOOTSTRAP_STAGE_RUNNING:
            if (next_state != UMI_BOOTSTRAP_STAGE_COMPLETE &&
                next_state != UMI_BOOTSTRAP_STAGE_FAILED) return UMI_STATUS_INVALID_STATE;
            break;
        default:
            return UMI_STATUS_INVALID_STATE;
    }
    stage->state = next_state;
    return UMI_STATUS_OK;
}
