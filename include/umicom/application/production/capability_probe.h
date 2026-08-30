/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/capability_probe.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_CAPABILITY_PROBE_H
#define UMICOM_APPLICATION_PRODUCTION_CAPABILITY_PROBE_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/capability_requirement.h"
#include "umicom/application/runtime/types.h"

typedef struct UmiApplicationProductionCapabilityProbeResult {
    const UmiApplicationProductionCapabilityRequirement *requirement;
    int available;
} UmiApplicationProductionCapabilityProbeResult;

typedef struct UmiApplicationProductionCapabilityProbeResults {
    UmiApplicationProductionCapabilityProbeResult
        entries[UMI_APPLICATION_PRODUCTION_MAX_CAPABILITIES];
    size_t count;
    size_t available_count;
    size_t unavailable_count;
    size_t critical_unavailable_count;
} UmiApplicationProductionCapabilityProbeResults;

UmiStatus umi_application_production_capability_probe_run(
    const UmiApplicationProductionCapabilityRequirements *requirements,
    UmiApplicationCapabilityProbe probe, void *user_data,
    UmiApplicationProductionCapabilityProbeResults *out_results);

#ifdef __cplusplus
}
#endif
#endif
