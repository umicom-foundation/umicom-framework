/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/thin_client.c
 *
 * PURPOSE:
 *   Implement the common thin-application composition over existing Framework
 *   contract, workspace runtime, readiness and health services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/thin_client.h"

#include <string.h>

/*
 * Initialise all common application state from one canonical application id.
 * No product-specific service, storage or workbench implementation is created.
 */
UmiStatus umi_application_thin_client_init(
    const char *application_id,
    UmiApplicationThinClient *out_client)
{
    UmiStatus status;

    if (application_id == NULL || out_client == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    memset(out_client, 0, sizeof(*out_client));
    out_client->structure_size = sizeof(*out_client);

    status = umi_application_contract_resolve(
        application_id, &out_client->contract);
    if (status != UMI_STATUS_OK)
        return status;

    status = umi_application_contract_validate(&out_client->contract);
    if (status != UMI_STATUS_OK)
        return status;

    status = umi_application_workspace_runtime_init(
        out_client->contract.experience, &out_client->workspace);
    if (status != UMI_STATUS_OK)
        return status;

    return umi_application_readiness_report(
        out_client->contract.experience, &out_client->readiness);
}

/* Delegate workbench binding to the existing Framework workspace runtime. */
UmiStatus umi_application_thin_client_bind_workbench(
    UmiApplicationThinClient *client,
    UmiUiWorkbench *workbench)
{
    if (client == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_workspace_runtime_bind_workbench(
        &client->workspace, workbench);
}

/* Delegate layout selection so one runtime owns layout/session transitions. */
UmiStatus umi_application_thin_client_select_layout(
    UmiApplicationThinClient *client,
    const char *layout_id)
{
    if (client == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_workspace_runtime_select_layout(
        &client->workspace, layout_id);
}

/* Delegate panel activation to the canonical workspace runtime. */
UmiStatus umi_application_thin_client_activate_panel(
    UmiApplicationThinClient *client,
    const char *panel_id)
{
    if (client == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_workspace_runtime_activate_panel(
        &client->workspace, panel_id);
}

/* Delegate panel deactivation to the canonical workspace runtime. */
UmiStatus umi_application_thin_client_deactivate_panel(
    UmiApplicationThinClient *client,
    const char *panel_id)
{
    if (client == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_workspace_runtime_deactivate_panel(
        &client->workspace, panel_id);
}

/* Keep product layout-lock requests on the existing Framework lock path. */
UmiStatus umi_application_thin_client_set_layout_locked(
    UmiApplicationThinClient *client,
    bool locked)
{
    if (client == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_workspace_runtime_set_layout_locked(
        &client->workspace, locked);
}

/* Publish product context through the canonical context binding service. */
UmiStatus umi_application_thin_client_set_context(
    UmiApplicationThinClient *client,
    const char *group_id,
    const char *value)
{
    if (client == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_workspace_runtime_set_context(
        &client->workspace, group_id, value);
}

/* Re-read readiness from immutable canonical experience metadata. */
UmiStatus umi_application_thin_client_refresh_readiness(
    UmiApplicationThinClient *client)
{
    if (client == NULL || client->contract.experience == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_readiness_report(
        client->contract.experience, &client->readiness);
}

/* Delegate capability health evaluation to the existing Framework evaluator. */
UmiStatus umi_application_thin_client_health(
    const UmiApplicationThinClient *client,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health)
{
    if (client == NULL || client->contract.experience == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_runtime_health_evaluate(
        client->contract.experience, probe, user_data, out_health);
}
