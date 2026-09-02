/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/lifecycle.h
 *
 * PURPOSE:
 *   Execute configure/build/rebuild/clean/test/run/install/package operations
 *   from named workbench configurations through Framework toolchain services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_LIFECYCLE_H
#define UMICOM_DEVELOPER_WORKBENCH_LIFECYCLE_H

#include "umicom/developer_workbench/configuration_registry.h"
#include "umicom/developer_workbench/operation_history.h"
#include "umicom/platform/cancellation.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench lifecycle data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkbenchLifecycle UmiDeveloperWorkbenchLifecycle;

/**
 * Initialise developer workbench lifecycle from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_workbench_lifecycle_create(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    UmiDeveloperWorkbenchConfigurationRegistry *configurations,
    UmiDeveloperWorkbenchOperationHistory *history,
    UmiDeveloperWorkbenchLifecycle **out_lifecycle);

/**
 * Release or reset state held by developer workbench lifecycle so the same storage can be
 * reused safely.
 */
void umi_developer_workbench_lifecycle_destroy(
    UmiDeveloperWorkbenchLifecycle *lifecycle);

/**
 * Perform developer workbench lifecycle through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_developer_workbench_lifecycle_execute(
    UmiDeveloperWorkbenchLifecycle *lifecycle,
    const char *command_id,
    UmiDeveloperWorkbenchCommandAction action,
    UmiBuildReport *out_report);

/**
 * Provide the developer workbench lifecycle cancel operation used by this module and its
 * client applications.
 */
void umi_developer_workbench_lifecycle_cancel(
    UmiDeveloperWorkbenchLifecycle *lifecycle);

/**
 * Provide the developer workbench lifecycle is cancel requested operation used by this
 * module and its client applications.
 */
int umi_developer_workbench_lifecycle_is_cancel_requested(
    const UmiDeveloperWorkbenchLifecycle *lifecycle);

/**
 * Provide the developer workbench lifecycle is running operation used by this module and
 * its client applications.
 */
int umi_developer_workbench_lifecycle_is_running(
    const UmiDeveloperWorkbenchLifecycle *lifecycle);

#ifdef __cplusplus
}
#endif

#endif
