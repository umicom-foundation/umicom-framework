/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/health.h
 *
 * PURPOSE:
 *   Summarise integration availability without assuming every optional application is installed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_HEALTH_H
#define UMICOM_INTEGRATION_HEALTH_H

#include "umicom/integration/launch_plan.h"

/**
 * Represent the integration health summary data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationHealthSummary {
    size_t available;
    size_t running;
    size_t missing_required;
    size_t missing_optional;
    bool healthy;
    bool degraded;
} UmiIntegrationHealthSummary;

/**
 * Provide the integration health from plan operation used by this module and its client
 * applications.
 */
void umi_integration_health_from_plan(
    const UmiIntegrationLaunchPlan *plan,
    UmiIntegrationHealthSummary *out_summary);

#endif
