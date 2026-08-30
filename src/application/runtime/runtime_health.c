/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/runtime_health.c
 *
 * PURPOSE:
 *   Evaluate blocked, degraded and ready application states from canonical evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/runtime_health.h"

#include <string.h>

UmiStatus umi_application_runtime_health_evaluate(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health)
{
    UmiApplicationReadinessReport readiness;
    UmiApplicationCapabilityStatus capabilities;
    UmiStatus result;
    if (experience == NULL || out_health == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_readiness_report(experience, &readiness);
    if (result != UMI_STATUS_OK) return result;
    result = umi_application_capability_status_build(experience, probe, user_data, &capabilities);
    if (result != UMI_STATUS_OK) return result;
    memset(out_health, 0, sizeof(*out_health));
    out_health->structure_size = sizeof(*out_health);
    out_health->readiness_percent = readiness.readiness_percent;
    out_health->unavailable_capabilities = capabilities.unavailable_count;
    out_health->open_p0_features = readiness.p0_open_count;
    out_health->open_p1_features = readiness.p1_open_count;
    if (capabilities.unavailable_count > 0U)
        out_health->severity = UMI_APPLICATION_HEALTH_BLOCKED;
    else if (readiness.p0_open_count > 0U || readiness.p1_open_count > 0U)
        out_health->severity = UMI_APPLICATION_HEALTH_DEGRADED;
    else
        out_health->severity = UMI_APPLICATION_HEALTH_READY;
    return UMI_STATUS_OK;
}

const char *umi_application_health_severity_text(UmiApplicationHealthSeverity severity)
{
    switch (severity) {
    case UMI_APPLICATION_HEALTH_READY: return "ready";
    case UMI_APPLICATION_HEALTH_DEGRADED: return "degraded";
    case UMI_APPLICATION_HEALTH_BLOCKED: return "blocked";
    default: return "unknown";
    }
}
