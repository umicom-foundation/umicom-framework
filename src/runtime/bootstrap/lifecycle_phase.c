/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/lifecycle_phase.c
 *
 * PURPOSE:
 *   Define stable ordering and text for service bootstrap and shutdown phases.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/lifecycle_phase.h"


const char *umi_bootstrap_lifecycle_phase_text(UmiBootstrapLifecyclePhase phase) {
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
int umi_bootstrap_lifecycle_phase_rank(UmiBootstrapLifecyclePhase phase) {
    return (phase >= UMI_BOOTSTRAP_PHASE_DISCOVER && phase <= UMI_BOOTSTRAP_PHASE_DESTROY)
        ? (int)phase : 0;
}
