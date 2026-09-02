/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_stage.c
 *
 * PURPOSE:
 *   Implement the bootstrap stage behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_stage.c
 *
 * PURPOSE:
 *   Track validated bootstrap-stage state transitions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_stage.h"


#include <string.h>
/*
 * Initialise bootstrap stage from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_bootstrap_stage_init(UmiBootstrapStage *stage,
                                   const char *stage_id,
                                   int32_t order) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stage == NULL || !umi_bootstrap_id_valid(stage_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(stage, 0, sizeof(*stage));
    status = umi_bootstrap_copy_text(stage->stage_id, sizeof(stage->stage_id), stage_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    stage->order = order;
    stage->state = UMI_BOOTSTRAP_STAGE_PENDING;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap stage transition operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_stage_transition(UmiBootstrapStage *stage,
                                         UmiBootstrapStageState next_state) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stage == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch (stage->state) {
        case UMI_BOOTSTRAP_STAGE_PENDING:
            /* Apply this operation only while the related capability or state is available. */
            if (next_state != UMI_BOOTSTRAP_STAGE_READY &&
                next_state != UMI_BOOTSTRAP_STAGE_SKIPPED &&
                next_state != UMI_BOOTSTRAP_STAGE_FAILED) return UMI_STATUS_INVALID_STATE;
            break;
        case UMI_BOOTSTRAP_STAGE_READY:
            /* Apply this branch only when its contract condition is satisfied. */
            if (next_state != UMI_BOOTSTRAP_STAGE_RUNNING &&
                next_state != UMI_BOOTSTRAP_STAGE_SKIPPED &&
                next_state != UMI_BOOTSTRAP_STAGE_FAILED) return UMI_STATUS_INVALID_STATE;
            break;
        case UMI_BOOTSTRAP_STAGE_RUNNING:
            /* Apply this branch only when its contract condition is satisfied. */
            if (next_state != UMI_BOOTSTRAP_STAGE_COMPLETE &&
                next_state != UMI_BOOTSTRAP_STAGE_FAILED) return UMI_STATUS_INVALID_STATE;
            break;
        default:
            return UMI_STATUS_INVALID_STATE;
    }
    stage->state = next_state;
    return UMI_STATUS_OK;
}
