/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/thin_client.h
 *
 * PURPOSE:
 *   Provide one Framework-owned composition helper for thin Umicom applications
 *   that need canonical experience, runtime, readiness and workbench services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_THIN_CLIENT_H
#define UMICOM_APPLICATION_THIN_CLIENT_H

#include "umicom/application/runtime/contract_resolver.h"
#include "umicom/application/runtime/readiness.h"
#include "umicom/application/runtime/runtime_health.h"
#include "umicom/application/runtime/workspace_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationThinClient {
    uint32_t structure_size;
    UmiApplicationContract contract;
    UmiApplicationWorkspaceRuntime workspace;
    UmiApplicationReadinessReport readiness;
} UmiApplicationThinClient;

/*
 * Resolve the canonical application definition/experience and initialise the
 * Framework-owned workspace runtime. Product modules use this instead of
 * repeating contract, runtime and readiness bootstrapping.
 */
UmiStatus umi_application_thin_client_init(
    const char *application_id,
    UmiApplicationThinClient *out_client);

/*
 * Bind an already-created Framework workbench. Ownership remains with the
 * caller; the thin client only projects canonical layouts, panels and context.
 */
UmiStatus umi_application_thin_client_bind_workbench(
    UmiApplicationThinClient *client,
    UmiUiWorkbench *workbench);

/* Select a canonical experience layout through the shared workspace runtime. */
UmiStatus umi_application_thin_client_select_layout(
    UmiApplicationThinClient *client,
    const char *layout_id);

/* Activate a canonical panel and synchronise it with a bound workbench. */
UmiStatus umi_application_thin_client_activate_panel(
    UmiApplicationThinClient *client,
    const char *panel_id);

/* Deactivate a canonical panel and synchronise it with a bound workbench. */
UmiStatus umi_application_thin_client_deactivate_panel(
    UmiApplicationThinClient *client,
    const char *panel_id);

/* Lock or unlock the selected layout using the existing Framework profile model. */
UmiStatus umi_application_thin_client_set_layout_locked(
    UmiApplicationThinClient *client,
    bool locked);

/* Publish a linked context value through the shared application/workbench runtime. */
UmiStatus umi_application_thin_client_set_context(
    UmiApplicationThinClient *client,
    const char *group_id,
    const char *value);

/*
 * Recompute feature readiness from the canonical experience definition after
 * Framework capability implementation state changes.
 */
UmiStatus umi_application_thin_client_refresh_readiness(
    UmiApplicationThinClient *client);

/*
 * Evaluate runtime health using a caller-supplied Framework capability probe.
 * The product module does not implement its own capability/health rules.
 */
UmiStatus umi_application_thin_client_health(
    const UmiApplicationThinClient *client,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health);

#ifdef __cplusplus
}
#endif
#endif
