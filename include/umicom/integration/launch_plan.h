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

/**
 * List the named integration launch disposition values accepted by this public contract.
 */
typedef enum UmiIntegrationLaunchDisposition {
    UMI_INTEGRATION_LAUNCH_READY = 0,
    UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING,
    UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING,
    UMI_INTEGRATION_LAUNCH_REQUIRED_MISSING,
    UMI_INTEGRATION_LAUNCH_DISABLED,

    /*
     * A launch plan returned by umi_integration_launch_plan_build() contains
     * only the original five values above. These additional values belong to
     * the per-suite COPY of that plan, held by UmiIntegrationSuiteRuntime.
     * They record explicit observations from the owning process supervisor.
     * Adding enum values changes neither existing numeric values nor the
     * layout of any public structure; this avoids a C ABI size change.
     */
    UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING,
    UMI_INTEGRATION_LAUNCH_OBSERVED_FAILED,

    /* R02 keeps launch attempts, clean exits and recoveries distinct from
     * binary availability. Append only: all earlier enum values, sizes and
     * field offsets remain ABI compatible. These dispositions exist only in
     * the existing runtime's copied plan, never in a fresh launch plan. */
    UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING,
    UMI_INTEGRATION_LAUNCH_OBSERVED_STOPPED
} UmiIntegrationLaunchDisposition;

/**
 * Represent the integration launch item data shared with callers of this public contract.
 */
typedef struct UmiIntegrationLaunchItem {
    char application_id[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationDependencyKind kind;
    UmiIntegrationLaunchDisposition disposition;
    unsigned preferred_frontend;
} UmiIntegrationLaunchItem;

/**
 * Represent the integration launch plan data shared with callers of this public contract.
 */
typedef struct UmiIntegrationLaunchPlan {
    UmiIntegrationLaunchItem items[UMI_INTEGRATION_MAX_MEMBERS];
    size_t count;
    size_t ready_count;
    size_t missing_required;
    size_t missing_optional;
} UmiIntegrationLaunchPlan;

/**
 * Provide the integration launch plan build operation used by this module and its client
 * applications.
 */
/*
 * The builder validates bounded member IDs/kinds/counts and rejects duplicate
 * members before publishing a complete plan. Absent REQUIRED members still
 * return UNAVAILABLE and publish the complete diagnostic plan, preserving the
 * established planning behaviour. Malformed source data leaves out_plan intact.
 */
UmiStatus umi_integration_launch_plan_build(
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry,
    UmiIntegrationLaunchPlan *out_plan);
/**
 * Provide the integration launch plan can start operation used by this module and its
 * client applications.
 */
bool umi_integration_launch_plan_can_start(
    const UmiIntegrationLaunchPlan *plan);

#endif
