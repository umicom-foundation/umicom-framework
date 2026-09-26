/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/runtime_bridge.c
 *
 * PURPOSE:
 *   Reuse the canonical runtime rather than introducing another launch or process engine.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/federation/runtime_bridge.h"
#include "umicom/desktop/desk_runtime.h"
#include "internal.h"
#include <string.h>

static UmiDeskFederationAppState ProjectState(UmiApplicationRuntimeState state)
{
    switch (state) {
    case UMI_APPLICATION_RUNTIME_STOPPED: return UMI_DESK_FEDERATION_STOPPED;
    case UMI_APPLICATION_RUNTIME_STARTING: return UMI_DESK_FEDERATION_STARTING;
    case UMI_APPLICATION_RUNTIME_RUNNING: return UMI_DESK_FEDERATION_RUNNING;
    case UMI_APPLICATION_RUNTIME_ATTENTION: return UMI_DESK_FEDERATION_ATTENTION;
    case UMI_APPLICATION_RUNTIME_FAILED: return UMI_DESK_FEDERATION_FAILED;
    case UMI_APPLICATION_RUNTIME_STOPPING: return UMI_DESK_FEDERATION_STOPPING;
    default: return UMI_DESK_FEDERATION_UNKNOWN;
    }
}
static UmiStatus ProjectRecord(const UmiApplicationRuntimeRecord *record, UmiDeskFederationApp *outApp)
{
    UmiDeskFederationApp app = {0};
    if (memchr(record->application_id, '\0', sizeof record->application_id) == NULL ||
        memchr(record->display_name, '\0', sizeof record->display_name) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (DfCopy(app.id.value, sizeof app.id.value, record->application_id) != UMI_STATUS_OK ||
        DfCopy(app.title, sizeof app.title, record->display_name) != UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    app.installed = record->installed; app.compatible = record->compatible;
    app.enabled = record->enabled; app.running = record->running;
    app.state = ProjectState(record->state); app.processToken = record->process_token;
    app.generation = record->generation;
    *outApp = app; return UMI_STATUS_OK;
}
static UmiStatus ReadRuntime(void *context, UmiDeskFederationCatalogue *outCatalogue)
{
    UmiApplicationRuntimeCatalogue *catalogue;
    UmiDeskFederationCatalogue result = {0};
    if (context == NULL || outCatalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    catalogue = umi_desk_runtime_applications(context);
    if (catalogue == NULL) return UMI_STATUS_INVALID_STATE;
    result.count = umi_application_runtime_catalogue_count(catalogue);
    if (result.count > UMI_DESK_FEDERATION_MAX_APPS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < result.count; ++index) {
        UmiApplicationRuntimeRecord record;
        UmiStatus status = umi_application_runtime_catalogue_at(catalogue, index, &record);
        if (status == UMI_STATUS_OK) status = ProjectRecord(&record, &result.apps[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    *outCatalogue = result; return UMI_STATUS_OK;
}
static UmiStatus RequestRuntime(void *context, const UmiDeskFederationApp *expected, UmiDeskFederationAction action)
{
    UmiApplicationRuntimeRecord record;
    UmiDeskFederationApp current;
    UmiStatus status;
    if (context == NULL || expected == NULL || !DfIdValid(expected->id.value, sizeof expected->id.value) ||
        (action != UMI_DESK_FEDERATION_START && action != UMI_DESK_FEDERATION_ACTIVATE))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_runtime_catalogue_find(umi_desk_runtime_applications(context), expected->id.value, &record);
    if (status == UMI_STATUS_OK) status = ProjectRecord(&record, &current);
    if (status != UMI_STATUS_OK) return status;
    if (!DfAppSame(expected, &current) ||
        (action == UMI_DESK_FEDERATION_START && current.running) ||
        (action == UMI_DESK_FEDERATION_ACTIVATE && !current.running)) return UMI_STATUS_BUSY;
    /* On this runtime's owning thread the check and semantic request run
     * without event-loop iteration. Existing readiness, process supervision,
     * executable-root checks and OS activation support stay authoritative. */
    return umi_desk_runtime_request_application(context, expected->id.value,
        UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE);
}
UmiDeskFederationPorts UmiDeskFederationRuntimePorts(UmiDeskRuntime *runtime)
{
    UmiDeskFederationPorts ports = {runtime, ReadRuntime, RequestRuntime};
    return ports;
}
UmiStatus UmiDeskFederationSaveSelection(UmiDeskFederation *service, UmiDeskRuntime *runtime,
    const char *id, const char *title)
{
    UmiApplicationLaunchSelectionCheckpoint checkpoint;
    UmiDeskWorkspace workspace = {0};
    UmiStatus status;
    if (service == NULL || runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workspace.kind = UMI_DESK_WORKSPACE_GROUP;
    status = DfCopy(workspace.id, sizeof workspace.id, id);
    if (status == UMI_STATUS_OK) status = DfCopy(workspace.title, sizeof workspace.title, title);
    if (status == UMI_STATUS_OK) status = umi_desk_runtime_capture_selection_checkpoint(runtime, &checkpoint);
    if (status != UMI_STATUS_OK) return status;
    if (checkpoint.selected_count == 0U) return UMI_STATUS_INVALID_STATE;
    if (checkpoint.selected_count > UMI_DESK_FEDERATION_MAX_APPS) return UMI_STATUS_CAPACITY_EXCEEDED;
    workspace.memberCount = checkpoint.selected_count;
    for (size_t index = 0U; index < workspace.memberCount; ++index) {
        if (memchr(checkpoint.application_ids[index], '\0', sizeof checkpoint.application_ids[index]) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        status = DfCopy(workspace.members[index].value, sizeof workspace.members[index].value, checkpoint.application_ids[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UmiDeskFederationSaveWorkspace(service, &workspace);
}
