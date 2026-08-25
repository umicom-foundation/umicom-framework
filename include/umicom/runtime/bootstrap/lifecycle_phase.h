/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/lifecycle_phase.h
 *
 * PURPOSE:
 *   Define stable ordering and text for service bootstrap and shutdown phases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_PHASE_H
#define UMICOM_RUNTIME_BOOTSTRAP_LIFECYCLE_PHASE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


const char *umi_bootstrap_lifecycle_phase_text(UmiBootstrapLifecyclePhase phase);
int umi_bootstrap_lifecycle_phase_rank(UmiBootstrapLifecyclePhase phase);

#ifdef __cplusplus
}
#endif

#endif
