/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/capability_probe.c
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
#include "umicom/application/production/capability_probe.h"

#include <string.h>

UmiStatus umi_application_production_capability_probe_run(
    const UmiApplicationProductionCapabilityRequirements *requirements,
    UmiApplicationCapabilityProbe probe, void *user_data,
    UmiApplicationProductionCapabilityProbeResults *out_results)
{
    size_t index;
    if (requirements == NULL || out_results == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_results, 0, sizeof(*out_results));
    for (index = 0U; index < requirements->count; ++index) {
        UmiApplicationProductionCapabilityProbeResult *result =
            &out_results->entries[out_results->count];
        result->requirement = &requirements->entries[index];
        result->available =
            probe != NULL &&
            probe(result->requirement->capability_id, user_data) != 0;
        out_results->available_count += (size_t)result->available;
        out_results->unavailable_count += (size_t)!result->available;
        if (!result->available &&
            result->requirement->critical_consumers > 0U)
            out_results->critical_unavailable_count += 1U;
        out_results->count += 1U;
    }
    return UMI_STATUS_OK;
}

