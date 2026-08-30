/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/capability_readiness.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/capability_readiness.h"

#include <string.h>

UmiStatus umi_application_production_capability_readiness_evaluate(
    const UmiApplicationProductionCapabilityProbeResults *results,
    UmiApplicationProductionCapabilityReadiness *out_readiness)
{
    if (results == NULL || out_readiness == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_readiness, 0, sizeof(*out_readiness));
    out_readiness->required_count = results->count;
    out_readiness->available_count = results->available_count;
    out_readiness->missing_count = results->unavailable_count;
    out_readiness->critical_missing_count =
        results->critical_unavailable_count;
    out_readiness->percent = results->count == 0U ? 100U :
        (unsigned)((results->available_count * 100U) / results->count);
    if (out_readiness->critical_missing_count > 0U)
        out_readiness->state = UMI_APPLICATION_PRODUCTION_BLOCKED;
    else if (out_readiness->missing_count > 0U)
        out_readiness->state = UMI_APPLICATION_PRODUCTION_DEGRADED;
    else
        out_readiness->state = UMI_APPLICATION_PRODUCTION_READY;
    return UMI_STATUS_OK;
}

int umi_application_production_capability_readiness_launchable(
    const UmiApplicationProductionCapabilityReadiness *readiness)
{
    return readiness != NULL &&
           readiness->state != UMI_APPLICATION_PRODUCTION_BLOCKED;
}

