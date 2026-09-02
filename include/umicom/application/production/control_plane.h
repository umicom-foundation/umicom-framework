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

/**
 * Represent the application production control plane data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionControlPlane {
    UmiApplicationProductionRegistry registry;
    UmiApplicationProductionPortfolio portfolio;
    UmiApplicationProductionPortfolioReport report;
    uint64_t revision;
    int initialised;
} UmiApplicationProductionControlPlane;

/**
 * Initialise application production control plane from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_production_control_plane_init(
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionControlPlane *out_control_plane);
/**
 * Provide the application production control plane refresh operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_control_plane_refresh(
    UmiApplicationProductionControlPlane *control_plane);
/**
 * Provide the application production control plane runtime operation used by this module
 * and its client applications.
 */
const UmiApplicationProductionRuntime *
umi_application_production_control_plane_runtime(
    const UmiApplicationProductionControlPlane *control_plane,
    const char *application_id);

#ifdef __cplusplus
}
#endif
#endif
