/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/runtime_health.h
 *
 * PURPOSE:
 *   Aggregate product readiness and capability availability into a runtime health decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_RUNTIME_HEALTH_H
#define UMICOM_APPLICATION_RUNTIME_RUNTIME_HEALTH_H

#include "umicom/application/runtime/capability_status.h"
#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationRuntimeHealth {
    uint32_t structure_size;
    UmiApplicationHealthSeverity severity;
    unsigned readiness_percent;
    size_t unavailable_capabilities;
    size_t open_p0_features;
    size_t open_p1_features;
} UmiApplicationRuntimeHealth;

UmiStatus umi_application_runtime_health_evaluate(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health);
const char *umi_application_health_severity_text(
    UmiApplicationHealthSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
