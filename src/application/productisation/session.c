/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/session.c
 *
 * PURPOSE:
 *   Implement a common product session so application modules only contribute
 *   identity while Framework owns layout, panel, context and readiness logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/session.h"

#include <stdio.h>
#include <string.h>

static int has_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static int session_valid(const UmiProductApplicationSession *session)
{
    return session != NULL &&
        session->structure_size >= sizeof(*session) &&
        session->initialised && session->adoption != NULL &&
        session->client.contract.experience != NULL;
}

static UmiStatus command_validate(
    const UmiProductApplicationSessionCommand *command)
{
    if (command == NULL || command->structure_size < sizeof(*command) ||
        command->kind < UMI_PRODUCT_SESSION_SELECT_LAYOUT ||
        command->kind > UMI_PRODUCT_SESSION_SYNCHRONISE_WORKBENCH)
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((command->kind == UMI_PRODUCT_SESSION_SELECT_LAYOUT ||
         command->kind == UMI_PRODUCT_SESSION_ACTIVATE_PANEL ||
         command->kind == UMI_PRODUCT_SESSION_DEACTIVATE_PANEL) &&
        !has_text(command->target_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (command->kind == UMI_PRODUCT_SESSION_SET_CONTEXT &&
        (!has_text(command->target_id) || command->value == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus umi_product_application_session_init(
    const UmiProductApplicationAdoption *adoption,
    UmiProductApplicationSession *out_session)
{
    UmiStatus status;
    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_application_adoption_validate(adoption);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_session, 0, sizeof(*out_session));
    out_session->structure_size = sizeof(*out_session);
    out_session->adoption = adoption;
    status = umi_product_application_adoption_snapshot(
        adoption, &out_session->adoption_snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_thin_client_init(
        adoption->application_id, &out_session->client);
    if (status != UMI_STATUS_OK) return status;
    out_session->last_status = UMI_STATUS_OK;
    out_session->revision = 1U;
    out_session->initialised = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_product_application_session_bind_workbench(
    UmiProductApplicationSession *session,
    UmiUiWorkbench *workbench)
{
    UmiStatus status;
    if (!session_valid(session) || workbench == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_thin_client_bind_workbench(
        &session->client, workbench);
    session->last_status = status;
    session->revision += 1U;
    return status;
}

UmiStatus umi_product_application_session_execute(
    UmiProductApplicationSession *session,
    const UmiProductApplicationSessionCommand *command)
{
    UmiStatus status;
    if (!session_valid(session)) return UMI_STATUS_INVALID_STATE;
    status = command_validate(command);
    if (status != UMI_STATUS_OK) return status;

    switch (command->kind) {
    case UMI_PRODUCT_SESSION_SELECT_LAYOUT:
        status = umi_application_thin_client_select_layout(
            &session->client, command->target_id);
        break;
    case UMI_PRODUCT_SESSION_ACTIVATE_PANEL:
        status = umi_application_thin_client_activate_panel(
            &session->client, command->target_id);
        break;
    case UMI_PRODUCT_SESSION_DEACTIVATE_PANEL:
        status = umi_application_thin_client_deactivate_panel(
            &session->client, command->target_id);
        break;
    case UMI_PRODUCT_SESSION_SET_CONTEXT:
        status = umi_application_thin_client_set_context(
            &session->client, command->target_id, command->value);
        break;
    case UMI_PRODUCT_SESSION_SET_LAYOUT_LOCKED:
        status = umi_application_thin_client_set_layout_locked(
            &session->client, command->locked);
        break;
    case UMI_PRODUCT_SESSION_REFRESH_READINESS:
        status = umi_application_thin_client_refresh_readiness(
            &session->client);
        break;
    case UMI_PRODUCT_SESSION_SYNCHRONISE_WORKBENCH:
        status = umi_application_workspace_runtime_sync_workbench(
            &session->client.workspace);
        break;
    default:
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
    }

    session->command_count += 1U;
    if (status == UMI_STATUS_OK) session->successful_command_count += 1U;
    else session->failed_command_count += 1U;
    session->last_status = status;
    session->revision += 1U;
    return status;
}

UmiStatus umi_product_application_session_snapshot(
    const UmiProductApplicationSession *session,
    UmiProductApplicationSessionSnapshot *out_snapshot)
{
    UmiStatus status;
    if (!session_valid(session) || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    status = copy_text(out_snapshot->module_id,
                       sizeof(out_snapshot->module_id),
                       session->adoption_snapshot.module_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->application_id,
                       sizeof(out_snapshot->application_id),
                       session->adoption_snapshot.application_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_snapshot->display_name,
                       sizeof(out_snapshot->display_name),
                       session->adoption_snapshot.display_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_session_snapshot_capture(
        &session->client.workspace.session, &out_snapshot->workspace);
    if (status != UMI_STATUS_OK) return status;

    out_snapshot->feature_count = session->adoption_snapshot.feature_count;
    out_snapshot->panel_count = session->adoption_snapshot.panel_count;
    out_snapshot->layout_count = session->adoption_snapshot.layout_count;
    out_snapshot->operation_count =
        session->client.workspace.operations.operation_count;
    out_snapshot->command_count = session->command_count;
    out_snapshot->successful_command_count =
        session->successful_command_count;
    out_snapshot->failed_command_count = session->failed_command_count;
    out_snapshot->readiness_percent = session->client.readiness.readiness_percent;
    out_snapshot->last_status = session->last_status;
    out_snapshot->revision = session->revision;
    out_snapshot->workbench_bound = session->client.workspace.workbench != NULL;
    out_snapshot->runnable = session->adoption_snapshot.runnable;
    out_snapshot->acceptance_ready =
        session->adoption_snapshot.acceptance_ready;
    return UMI_STATUS_OK;
}

UmiStatus umi_product_application_session_health(
    const UmiProductApplicationSession *session,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationRuntimeHealth *out_health)
{
    if (!session_valid(session)) return UMI_STATUS_INVALID_STATE;
    return umi_application_thin_client_health(
        &session->client, probe, user_data, out_health);
}

UmiStatus umi_product_application_session_reset(
    UmiProductApplicationSession *session)
{
    const UmiProductApplicationAdoption *adoption;
    if (!session_valid(session)) return UMI_STATUS_INVALID_STATE;
    adoption = session->adoption;
    return umi_product_application_session_init(adoption, session);
}
