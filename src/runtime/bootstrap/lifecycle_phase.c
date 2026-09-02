/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/lifecycle_phase.c
 *
 * PURPOSE:
 *   Implement the lifecycle phase behavior for
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
 * File: src/runtime/bootstrap/lifecycle_phase.c
 *
 * PURPOSE:
 *   Define stable ordering and text for service bootstrap and shutdown phases.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/lifecycle_phase.h"


/*
 * Provide the bootstrap lifecycle phase text operation used by this module and its client
 * applications.
 */
const char *umi_bootstrap_lifecycle_phase_text(UmiBootstrapLifecyclePhase phase) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (phase) {
        case UMI_BOOTSTRAP_PHASE_DISCOVER: return "discover";
        case UMI_BOOTSTRAP_PHASE_CONFIGURE: return "configure";
        case UMI_BOOTSTRAP_PHASE_CREATE: return "create";
        case UMI_BOOTSTRAP_PHASE_START: return "start";
        case UMI_BOOTSTRAP_PHASE_READY: return "ready";
        case UMI_BOOTSTRAP_PHASE_STOP: return "stop";
        case UMI_BOOTSTRAP_PHASE_DESTROY: return "destroy";
        default: return "unknown";
    }
}
/*
 * Provide the bootstrap lifecycle phase rank operation used by this module and its client
 * applications.
 */
int umi_bootstrap_lifecycle_phase_rank(UmiBootstrapLifecyclePhase phase) {
    return (phase >= UMI_BOOTSTRAP_PHASE_DISCOVER && phase <= UMI_BOOTSTRAP_PHASE_DESTROY)
        ? (int)phase : 0;
}
