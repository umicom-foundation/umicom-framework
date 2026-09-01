/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/launch_plan.h
 *
 * PURPOSE:
 *   Build deterministic suite launch plans while tolerating absent optional products.
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

#ifndef UMICOM_INTEGRATION_LAUNCH_PLAN_H
#define UMICOM_INTEGRATION_LAUNCH_PLAN_H

#include "umicom/base/status.h"
#include "umicom/integration/registry.h"
#include "umicom/integration/suite.h"

typedef enum UmiIntegrationLaunchDisposition {
    UMI_INTEGRATION_LAUNCH_READY = 0,
    UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING,
    UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING,
    UMI_INTEGRATION_LAUNCH_REQUIRED_MISSING,
    UMI_INTEGRATION_LAUNCH_DISABLED
} UmiIntegrationLaunchDisposition;

typedef struct UmiIntegrationLaunchItem {
    char application_id[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationDependencyKind kind;
    UmiIntegrationLaunchDisposition disposition;
    unsigned preferred_frontend;
} UmiIntegrationLaunchItem;

typedef struct UmiIntegrationLaunchPlan {
    UmiIntegrationLaunchItem items[UMI_INTEGRATION_MAX_MEMBERS];
    size_t count;
    size_t ready_count;
    size_t missing_required;
    size_t missing_optional;
} UmiIntegrationLaunchPlan;

UmiStatus umi_integration_launch_plan_build(
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry,
    UmiIntegrationLaunchPlan *out_plan);
bool umi_integration_launch_plan_can_start(
    const UmiIntegrationLaunchPlan *plan);

#endif
