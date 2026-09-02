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

/*
 * Provide the application runtime health evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_runtime_health_evaluate(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health)
{
    UmiApplicationReadinessReport readiness;
    UmiApplicationCapabilityStatus capabilities;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_health == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_readiness_report(experience, &readiness);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    result = umi_application_capability_status_build(experience, probe, user_data, &capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    memset(out_health, 0, sizeof(*out_health));
    out_health->structure_size = sizeof(*out_health);
    out_health->readiness_percent = readiness.readiness_percent;
    out_health->unavailable_capabilities = capabilities.unavailable_count;
    out_health->open_p0_features = readiness.p0_open_count;
    out_health->open_p1_features = readiness.p1_open_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (capabilities.unavailable_count > 0U)
        out_health->severity = UMI_APPLICATION_HEALTH_BLOCKED;
    else /* Apply this branch only when its contract condition is satisfied. */ if (readiness.p0_open_count > 0U || readiness.p1_open_count > 0U)
        out_health->severity = UMI_APPLICATION_HEALTH_DEGRADED;
    /* Use this fallback path when the earlier condition does not apply. */
    else
        out_health->severity = UMI_APPLICATION_HEALTH_READY;
    return UMI_STATUS_OK;
}

/*
 * Provide the application health severity text operation used by this module and its
 * client applications.
 */
const char *umi_application_health_severity_text(UmiApplicationHealthSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
    case UMI_APPLICATION_HEALTH_READY: return "ready";
    case UMI_APPLICATION_HEALTH_DEGRADED: return "degraded";
    case UMI_APPLICATION_HEALTH_BLOCKED: return "blocked";
    default: return "unknown";
    }
}
