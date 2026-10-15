/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/bootstrap_stage.h
 *
 * PURPOSE:
 *   Track validated bootstrap-stage state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_STAGE_H
#define UMICOM_RUNTIME_BOOTSTRAP_BOOTSTRAP_STAGE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap stage from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_stage_init(UmiBootstrapStage *stage,
                                   const char *stage_id,
                                   int32_t order);
/**
 * Provide the bootstrap stage transition operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_stage_transition(UmiBootstrapStage *stage,
                                         UmiBootstrapStageState next_state);

#ifdef __cplusplus
}
#endif

#endif
