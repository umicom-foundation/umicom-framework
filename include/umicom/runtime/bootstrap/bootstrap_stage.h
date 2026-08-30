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


UmiStatus umi_bootstrap_stage_init(UmiBootstrapStage *stage,
                                   const char *stage_id,
                                   int32_t order);
UmiStatus umi_bootstrap_stage_transition(UmiBootstrapStage *stage,
                                         UmiBootstrapStageState next_state);

#ifdef __cplusplus
}
#endif

#endif
