/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/lifecycle_gate.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/lifecycle_gate.h"
#include "umicom/application/production/launch_stage.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_application_production_lifecycle_gate_evaluate(
    UmiApplicationProductionGate gate,
    const UmiApplicationProductionBinding *binding,
    const UmiApplicationProductionReadinessReport *readiness,
    const UmiApplicationProductionManifestDrift *drift,
    const UmiApplicationProductionRecoveryReport *recovery,
    const UmiApplicationProductionAcceptanceReport *acceptance,
    UmiApplicationProductionLifecycleGate *out_gate)
{
    UmiStatus status;
    int written;
    if (readiness == NULL || drift == NULL || recovery == NULL ||
        acceptance == NULL || out_gate == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_gate, 0, sizeof(*out_gate));
    out_gate->gate = gate;
    switch (gate) {
    case UMI_APPLICATION_PRODUCTION_GATE_CONTRACT:
        out_gate->passed = 1;
        break;
    case UMI_APPLICATION_PRODUCTION_GATE_MANIFEST:
        out_gate->passed = drift->compatible;
        break;
    case UMI_APPLICATION_PRODUCTION_GATE_CAPABILITY:
        out_gate->passed =
            umi_application_production_capability_readiness_launchable(
                &readiness->capabilities);
        break;
    case UMI_APPLICATION_PRODUCTION_GATE_RECOVERY:
        out_gate->passed =
            recovery->status == UMI_STATUS_OK ||
            !recovery->checkpoint_found;
        break;
    case UMI_APPLICATION_PRODUCTION_GATE_ACCEPTANCE:
        out_gate->passed = acceptance->accepted;
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_gate->state = out_gate->passed
        ? (readiness->state == UMI_APPLICATION_PRODUCTION_DEGRADED
           ? UMI_APPLICATION_PRODUCTION_DEGRADED
           : UMI_APPLICATION_PRODUCTION_READY)
        : UMI_APPLICATION_PRODUCTION_BLOCKED;
    written = snprintf(out_gate->reason, sizeof(out_gate->reason),
        "%s gate %s for %s",
        umi_application_production_gate_text(gate),
        out_gate->passed ? "passed" : "blocked",
        binding->experience->application_id);
    if (written < 0 || (size_t)written >= sizeof(out_gate->reason))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
