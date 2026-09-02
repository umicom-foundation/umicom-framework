/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/lifecycle_phase.h
 *
 * PURPOSE:
 *   Define stable ordering and text for service bootstrap and shutdown phases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_PHASE_H
#define UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_PHASE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap lifecycle phase text operation used by this module and its client
 * applications.
 */
const char *umi_bootstrap_lifecycle_phase_text(UmiBootstrapLifecyclePhase phase);
/**
 * Provide the bootstrap lifecycle phase rank operation used by this module and its client
 * applications.
 */
int umi_bootstrap_lifecycle_phase_rank(UmiBootstrapLifecyclePhase phase);

#ifdef __cplusplus
}
#endif

#endif
