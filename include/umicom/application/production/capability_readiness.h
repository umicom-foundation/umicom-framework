/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/capability_readiness.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_CAPABILITY_READINESS_H
#define UMICOM_APPLICATION_PRODUCTION_CAPABILITY_READINESS_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/capability_probe.h"

typedef struct UmiApplicationProductionCapabilityReadiness {
    UmiApplicationProductionState state;
    size_t required_count;
    size_t available_count;
    size_t missing_count;
    size_t critical_missing_count;
    unsigned percent;
} UmiApplicationProductionCapabilityReadiness;

UmiStatus umi_application_production_capability_readiness_evaluate(
    const UmiApplicationProductionCapabilityProbeResults *results,
    UmiApplicationProductionCapabilityReadiness *out_readiness);
int umi_application_production_capability_readiness_launchable(
    const UmiApplicationProductionCapabilityReadiness *readiness);

#ifdef __cplusplus
}
#endif
#endif
