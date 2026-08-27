/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/lifecycle_gate.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_LIFECYCLE_GATE_H
#define UMICOM_APPLICATION_PRODUCTION_LIFECYCLE_GATE_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/acceptance_report.h"
#include "umicom/application/production/workspace_recovery.h"

typedef struct UmiApplicationProductionLifecycleGate {
    UmiApplicationProductionGate gate;
    UmiApplicationProductionState state;
    int passed;
    char reason[UMI_APPLICATION_PRODUCTION_TEXT_CAPACITY];
} UmiApplicationProductionLifecycleGate;

UmiStatus umi_application_production_lifecycle_gate_evaluate(
    UmiApplicationProductionGate gate,
    const UmiApplicationProductionBinding *binding,
    const UmiApplicationProductionReadinessReport *readiness,
    const UmiApplicationProductionManifestDrift *drift,
    const UmiApplicationProductionRecoveryReport *recovery,
    const UmiApplicationProductionAcceptanceReport *acceptance,
    UmiApplicationProductionLifecycleGate *out_gate);

#ifdef __cplusplus
}
#endif
#endif
