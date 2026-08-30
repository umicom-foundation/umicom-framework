/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/control_plane.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_CONTROL_PLANE_H
#define UMICOM_APPLICATION_PRODUCTION_CONTROL_PLANE_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/diagnostic_report.h"
#include "umicom/application/production/portfolio_report.h"
#include "umicom/application/production/registry.h"

typedef struct UmiApplicationProductionControlPlane {
    UmiApplicationProductionRegistry registry;
    UmiApplicationProductionPortfolio portfolio;
    UmiApplicationProductionPortfolioReport report;
    uint64_t revision;
    int initialised;
} UmiApplicationProductionControlPlane;

UmiStatus umi_application_production_control_plane_init(
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionControlPlane *out_control_plane);
UmiStatus umi_application_production_control_plane_refresh(
    UmiApplicationProductionControlPlane *control_plane);
const UmiApplicationProductionRuntime *
umi_application_production_control_plane_runtime(
    const UmiApplicationProductionControlPlane *control_plane,
    const char *application_id);

#ifdef __cplusplus
}
#endif
#endif
