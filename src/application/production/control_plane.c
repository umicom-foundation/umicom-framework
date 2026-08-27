/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/control_plane.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/control_plane.h"

#include <string.h>

UmiStatus umi_application_production_control_plane_init(
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionControlPlane *out_control_plane)
{
    size_t index;
    UmiStatus status;
    if (out_control_plane == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_control_plane, 0, sizeof(*out_control_plane));
    umi_application_production_registry_init(&out_control_plane->registry);
    status = umi_application_production_portfolio_build(
        probe, probe_context, &out_control_plane->portfolio);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < out_control_plane->portfolio.count; ++index) {
        status = umi_application_production_registry_register(
            &out_control_plane->registry,
            &out_control_plane->portfolio.entries[index].adoption);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_application_production_portfolio_report_build(
        &out_control_plane->portfolio, &out_control_plane->report);
    if (status != UMI_STATUS_OK) return status;
    out_control_plane->revision = 1U;
    out_control_plane->initialised = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_production_control_plane_refresh(
    UmiApplicationProductionControlPlane *control_plane)
{
    UmiStatus status;
    if (control_plane == NULL || !control_plane->initialised)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_portfolio_report_build(
        &control_plane->portfolio, &control_plane->report);
    if (status == UMI_STATUS_OK) control_plane->revision += 1U;
    return status;
}

const UmiApplicationProductionRuntime *
umi_application_production_control_plane_runtime(
    const UmiApplicationProductionControlPlane *control_plane,
    const char *application_id)
{
    const UmiApplicationProductionPortfolioEntry *entry;
    if (control_plane == NULL || !control_plane->initialised)
        return NULL;
    entry = umi_application_production_portfolio_find(
        &control_plane->portfolio, application_id);
    return entry != NULL ? &entry->runtime : NULL;
}

